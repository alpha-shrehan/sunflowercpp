#pragma once

#include "../header.hpp"
#include "../parser/ast.hpp"

namespace sf
{
    class stack_node;
    class Module;
    class SF_API name_lookup
    {
        int lowest_st_idx = 0;
    public:
        std::map<std::string, stack_node *> val;

        name_lookup() = default;
        ~name_lookup();

        void set(std::string, stack_node *);
        stack_node *get(std::string);
    };

    /* `r' stands for recursive */
    SF_API stack_node *get_var_from_mod_r(std::string, Module *);
    SF_API void set_var_to_mod_r(std::string, stack_node *, Module *);
} // namespace sf
