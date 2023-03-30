#include "name_lookup.hpp"
#include "module.hpp"

namespace sf
{
    void name_lookup::set(std::string s, stack_node *n)
    {
        if (lowest_st_idx == -1 || n->st_idx < lowest_st_idx)
            lowest_st_idx = n->st_idx;

        if (val.find(s) != val.end())
        {
            val[s]->ref_count--;
        }
        val[s] = n;
        n->ref_count++;
    }

    stack_node *name_lookup::get(std::string s)
    {
        if (val.find(s) != val.end())
        {
            return val[s];
        }
        return nullptr;
    }

    SF_API stack_node *
    get_var_from_mod_r(std::string s, Module *mod)
    {
        stack_node *node = mod->names.get(s);
        if (node != nullptr)
            return node;
        else
        {
            if (mod->parent != nullptr)
                return get_var_from_mod_r(s, mod->parent);
            else
                return nullptr;
        }
    }

    SF_API void
    set_var_to_mod_r(std::string name, stack_node *val, Module *mod)
    {
        Module *target_mod = mod;
        Module *mod_top = mod;

        while (mod_top->parent != nullptr)
        {
            mod_top = mod_top->parent;

            if (mod_top->names.get(name) != nullptr)
                target_mod = mod_top;
        }

        target_mod->names.set(name, val);
    }

    name_lookup::~name_lookup()
    {
        gstack_set_bp(lowest_st_idx);
        for (auto &n : val)
        {
            n.second->ref_count--;
        }
        
        gstack_pop_bp();
        val.clear();
    }
} // namespace sf
