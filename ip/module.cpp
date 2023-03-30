#include "module.hpp"

namespace sf
{
    /**
     * @brief The entire program uses a single stack.
     *
     */
    std::map<stack_node *, bool> existance_stack;
    std::vector<stack_node *> stack;
    std::stack<int> stack_bp;

    SF_API std::vector<stack_node *> getStack(void)
    {
        return stack;
    }

    SF_API stack_node *add_to_stack(stack_node *n)
    {
        int exists = -1;

        for (size_t i = 0; i < stack.size(); i++)
        {
            if (stack[i]->val->id == n->val->id)
            {
                exists = i;
                break;
            }
        }

        // std::cout << "exists: " << exists << std::endl;

        if (exists != -1)
        {
            n->val = nullptr;
            delete n;
            return stack[exists];
        }

        if (existance_stack.find(n) != existance_stack.end())
        {
            return n;
        }

        n->val->anonymous = false;
        stack.push_back(n);
        existance_stack[n] = true;
        n->st_idx = stack.size() - 1;
        return n;
    }

    SF_API void 
    gstack_clear(void)
    {
        stack.clear();
        existance_stack.clear();
        stack_bp = std::stack<int>();
    }

    SF_API stack_node *remove_from_stack(stack_node *n)
    {
        if (existance_stack.find(n) == existance_stack.end())
        {
            return n;
        }
        stack.erase(stack.begin() + n->st_idx);
        existance_stack.erase(n);

        for (int i = n->st_idx; i < stack.size(); i++)
        {
            stack[i]->st_idx--;
        }
        
        return n;
    }

    stack_node::stack_node(
        ast_expression *v,
        int rc)
    {
        val = v;
        ref_count = rc;
        st_idx = -1;
    }

    SF_API void
    gstack_set_bp(void)
    {
        stack_bp.push(stack.size());
    }

    SF_API void 
    gstack_set_bp(int i)
    {
        stack_bp.push(i);
    }

    SF_API void 
    gstack_pop_bp(void)
    {
        if (stack_bp.empty())
            return;

        int bp = stack_bp.top();
        stack_bp.pop();
        for (int i = bp; i < stack.size(); i++)
        {
            stack_node *n = stack[i];
            if (n->ref_count == 0)
            {
                remove_from_stack(n);
                delete n;
            }
        }
    }
} // namespace sf
