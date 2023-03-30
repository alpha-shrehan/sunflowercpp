#include "ip.hpp"

namespace sf
{
    SF_API bool
    expr_to_cbool(ast_expression *expr)
    {
        switch (expr->expr_type)
        {
        case AST_EXPR_INT:
        {
            ast_expr_int *int_expr = (ast_expr_int *)expr;
            return int_expr->value != 0;
        }
        break;
        case AST_EXPR_FLOAT:
        {
            ast_expr_float *float_expr = (ast_expr_float *)expr;
            return float_expr->value != 0.0f;
        }
        break;
        case AST_EXPR_STRING:
        {
            ast_expr_string *string_expr = (ast_expr_string *)expr;
            return string_expr->value != "";
        }
        break;
        case AST_EXPR_BOOL:
        {
            ast_expr_bool *bool_expr = (ast_expr_bool *)expr;
            return bool_expr->value;
        }
        break;
        default:
            return false;
        }
    }

    SF_API void ip_eval_tree(Module *mod, int *state_flag)
    {
        for (
            size_t i = 0;
            (i < mod->ast_nodes.size()) and
            *state_flag == STATE_OK;
            i++)
        {
            ast *node = mod->ast_nodes[i];
            switch (node->type)
            {
            case AST_VAR_DECL:
            {
                ast_var_decl *vd = (ast_var_decl *)node;
                ip_eval_rule_vardecl(vd, mod, state_flag);
            }
            break;
            case AST_FUNC_CALL:
            {
                ast_func_call *fc = (ast_func_call *)node;
                ip_eval_rule_funcall(fc, mod, state_flag);
            }
            break;
            case AST_IF:
            {
                ast_if *ifnode = (ast_if *)node;
                ip_eval_rule_stmt_if(ifnode, mod, state_flag);
            }
            break;
            default:
                break;
            }
        }

        if (state_flag != nullptr)
        {
            if (*state_flag != STATE_OK)
            {
                // printf("Error: %s\n", mod->error_msg.c_str()); // Cool concept suggested by AI
                printf("Error: %s\n", "Error message not implemented yet");
            }
        }
    }

    SF_API ast_expression *
    ip_eval_expr(ast_expression *expr, Module *mod, int *state_flag)
    {
        ast_expression *res = nullptr;

        switch (expr->expr_type)
        {
        case AST_EXPR_VAR:
        {
            ast_expr_var *var = (ast_expr_var *)expr;
            stack_node *node = get_var_from_mod_r(var->name, mod);
            if (node != nullptr)
            {
                res = node->val;
            }
            else
            {
                if (state_flag != nullptr)
                    *state_flag = STATE_ERROR;

                goto end;
            }
        }
        break;
        case AST_EXPR_INT:
        {
            if (not expr->evaluated)
                res = (ast_expression *)new ast_expr_int(((ast_expr_int *)expr)->value);
            else
                res = expr;
        }
        break;
        case AST_EXPR_FLOAT:
        {
            if (not expr->evaluated)
                res = (ast_expression *)new ast_expr_float(((ast_expr_float *)expr)->value);
            else
                res = expr;
        }
        break;
        case AST_EXPR_STRING:
        {
            if (not expr->evaluated)
                res = (ast_expression *)new ast_expr_string(((ast_expr_string *)expr)->value);
            else
                res = expr;
        }
        break;
        case AST_EXPR_BOOL:
        {
            if (not expr->evaluated)
                res = (ast_expression *)new ast_expr_bool(((ast_expr_bool *)expr)->value);
            else
                res = expr;
        }
        break;
        case AST_EXPR_ARRAY:
        {
            ast_expr_array *arr = (ast_expr_array *)expr;

            if (not arr->evaluated)
            {
                std::vector<stack_node *> eval_arr;

                for (size_t i = 0; i < arr->value.size(); i++)
                {
                    ast_expression *ev_exp = ip_eval_expr(arr->value[i]->val, mod, state_flag);

                    if (state_flag != nullptr and
                        *state_flag != STATE_OK)
                        goto end;

                    eval_arr.push_back(add_to_stack(new stack_node(ev_exp, 1)));
                }

                res = (ast_expression *)new ast_expr_array(eval_arr);
            }
            else
            {
                res = arr;
            }
        }
        break;
        case AST_EXPR_SQUARE_ACCESS:
        {
            ast_expr_square_access *ssa = (ast_expr_square_access *)expr;
            ast_expression *lval = ip_eval_expr(ssa->lval, mod, state_flag);
            ast_expression *index = ip_eval_expr(ssa->index, mod, state_flag);

            switch (lval->expr_type)
            {
            case AST_EXPR_ARRAY:
            {
                ast_expr_array *arr = (ast_expr_array *)lval;
                assert(index->expr_type == AST_EXPR_INT);
                ast_expr_int *int_index = (ast_expr_int *)index;

                if (int_index->value < arr->value.size())
                    res = arr->value[int_index->value]->val;
                else
                {
                    if (state_flag != nullptr)
                        *state_flag = STATE_ERROR;
                }
            }
            break;

            default:
                break;
            }

            if (lval->anonymous)
            {
                delete lval;
            }
            if (index->anonymous)
            {
                delete index;
            }
        }
        break;

        default:
            break;
        }

    end:
        if (res != nullptr)
            res->evaluated = true;
        return res;
    }

    SF_API void
    ip_eval_rule_vardecl(ast_var_decl *vd, Module *mod, int *state_flag)
    {
        ast_expression *name = vd->name;
        ast_expression *value = ip_eval_expr(vd->value, mod, state_flag);

        if (value == nullptr)
        {
            if (state_flag != nullptr)
                *state_flag = STATE_ERROR;
            return;
        }

        switch (name->expr_type)
        {
        case AST_EXPR_VAR:
        {
            ast_expr_var *var = (ast_expr_var *)name;
            set_var_to_mod_r(var->name, add_to_stack(new stack_node(value, 0)), mod);
        }
        break;

        default:
        {
            if (state_flag != nullptr)
                *state_flag = STATE_ERROR;
        }
        break;
        }

        if (state_flag != nullptr)
            *state_flag = STATE_OK;
    }

    SF_API void
    ip_eval_rule_funcall(ast_func_call *fc, Module *mod, int *state_flag)
    {
        ast_expression *name = ip_eval_expr(fc->name, mod, state_flag);

        if (name == nullptr)
            return;

        switch (name->expr_type)
        {
        case AST_EXPR_FUNCTION:
        {
            ast_expr_function *func = (ast_expr_function *)name;
            function *f = func->value;

            std::vector<ast_expression *> eval_args;

            for (size_t i = 0; i < fc->args.size(); i++)
            {
                ast_expression *ev_exp = ip_eval_expr(fc->args[i], mod, state_flag);

                if (state_flag != nullptr and
                    *state_flag != STATE_OK)
                    goto flag_error;

                eval_args.push_back(ev_exp);
            }

            Module *nd = new Module(MOD_TYPE_FUNC);
            nd->parent = mod;
            if (not f->isnative)
                nd->ast_nodes = f->body;

            gstack_set_bp();
            for (size_t i = 0; i < f->args.size(); i++)
            {
                nd->names.set(f->args[i], add_to_stack(new stack_node(eval_args[i], 0)));
            }

            if (f->isnative)
            {
                /* free the allocated return value. */
                delete f->native(nd, state_flag);

                if (state_flag != nullptr and
                    *state_flag != STATE_OK)
                    goto flag_error;
            }
            else
            {
                ip_eval_tree(nd, state_flag);

                if (state_flag != nullptr and
                    *state_flag != STATE_OK)
                    goto flag_error;
            }

            delete nd;
            gstack_pop_bp();
        }
        break;

        default:
        {
        flag_error:
            if (state_flag != nullptr)
                *state_flag = STATE_ERROR;
        }
        break;
        }
    }

    SF_API void
    ip_eval_rule_stmt_if(ast_if *ifnode, Module *mod, int *state_flag)
    {
        ast_expression *cond = ip_eval_expr(ifnode->condition, mod, state_flag);
        // ast_expression *cond = ifnode->condition;
        bool cond_val = expr_to_cbool(cond);

        if (cond_val)
        {
            Module *nd = new Module(MOD_TYPE_FILE);
            nd->parent = mod;
            nd->ast_nodes = ifnode->body;

            ip_eval_tree(nd, state_flag);

            delete nd;
        }
        else
        {
            bool done = false;

            if (ifnode->elifs.size() > 0)
            {
                for (size_t i = 0; i < ifnode->elifs.size(); i++)
                {
                    std::pair<sf::ast_expression *, std::vector<sf::ast *>> elif = ifnode->elifs[i];

                    ast_expression *cond = ip_eval_expr(elif.first, mod, state_flag);
                    // ast_expression *cond = elif->condition;
                    bool cond_val = expr_to_cbool(cond);

                    if (cond_val)
                    {
                        Module *nd = new Module(MOD_TYPE_FILE);
                        nd->parent = mod;
                        nd->ast_nodes = elif.second;

                        ip_eval_tree(nd, state_flag);

                        delete nd;

                        done = true;
                        break;
                    }
                }
            }

            if (not done and ifnode->else_body.size() > 0)
            {
                Module *nd = new Module(MOD_TYPE_FILE);
                nd->parent = mod;
                nd->ast_nodes = ifnode->else_body;

                ip_eval_tree(nd, state_flag);

                delete nd;
            }
        }

        delete cond;

        if (state_flag != nullptr)
            *state_flag = STATE_OK;
    }
} // namespace sf
