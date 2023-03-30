#pragma once

#include "../header.hpp"
#include "../linkedlist.hpp"
#include "../parser/ast.hpp"
#include "name_lookup.hpp"

enum ModuleType
{
    MOD_TYPE_FILE,
    MOD_TYPE_FUNC,
    MOD_TYPE_CLASS,
    MOD_TYPE_INTERACTIVE,
};

namespace sf
{
    class SF_API stack_node
    {
    public:
        ast_expression *val;
        int ref_count;
        int st_idx; /* id of node in stack memory */

        stack_node(ast_expression *v, int rc);
        stack_node()
        {
            val = nullptr;
            ref_count = 0;
            st_idx = -1;
        }

        ~stack_node()
        {
            if (val != nullptr)
                delete val;
        }
    };

    SF_API std::vector<stack_node *> getStack(void);
    SF_API stack_node *add_to_stack(stack_node *);
    SF_API stack_node *remove_from_stack(stack_node *);
    SF_API void gstack_set_bp(void);
    SF_API void gstack_set_bp(int);
    SF_API void gstack_pop_bp(void);
    SF_API void gstack_clear(void);

    class SF_API Module
    {
    public:
        ModuleType type;
        name_lookup names;
        std::vector<ast *> ast_nodes;

        Module *parent;

        Module(ModuleType type) : type(type) { parent = nullptr; };
        Module() : type(MOD_TYPE_FILE) { parent = nullptr; };

        ~Module()
        {
        }
    };
} // namespace sf
