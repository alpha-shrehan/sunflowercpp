#include "ast.hpp"
#include "../ip/module.hpp"

namespace sf
{
    SF_API std::vector<ast *> parse_ast(std::vector<token *> tokens)
    {
        std::vector<ast *> asts;

        for (size_t i = 0; i < tokens.size(); i++)
        {
            token *c = tokens[i];

            switch (c->type)
            {
            case TOKEN_KEYWORD:
            {
                tok_keyword *k = (tok_keyword *)c;

                switch (k->value)
                {
                case KEYWORD_IF:
                {
                    ast_expression *if_cond = nullptr;
                    std::vector<token *> if_body;
                    std::vector<token *> else_body;
                    std::vector<std::pair<ast_expression *, std::vector<ast *>>> elifs;

                    int tab_count = get_tab_count(tokens, i);
                    int gb = 0;
                    int block_st = -1;

                    for (size_t j = i + 1; j < tokens.size(); j++)
                    {
                        token *t = tokens[j];

                        if (t->type == TOKEN_OPERATOR)
                        {
                            tok_operator *o = (tok_operator *)t;

                            if (o->value == "(" or
                                o->value == "{" or
                                o->value == "[")
                            {
                                gb++;
                            }
                            else if (o->value == ")" or
                                     o->value == "}" or
                                     o->value == "]")
                            {
                                gb--;
                            }
                        }

                        if (t->type == TOKEN_NEWLINE and not gb)
                        {
                            block_st = j;
                            break;
                        }
                    }

                    if_cond = parse_expression(tokens, i + 1, block_st);
                    if_body = get_block(tokens, block_st, tab_count);
                    block_st += if_body.size();

                    while (block_st < tokens.size())
                    {
                        if (tokens[block_st]->type == TOKEN_NEWLINE or
                            tokens[block_st]->type == TOKEN_TABSPACE)
                        {
                            block_st++;
                            continue;
                        }
                        break;
                    }

                el_l1:
                    if (block_st < tokens.size())
                    {
                        if (tokens[block_st]->type == TOKEN_KEYWORD)
                        {
                            tok_keyword *k = (tok_keyword *)tokens[block_st];

                            if (k->value == KEYWORD_ELSE)
                            {
                                bool saw_elif = false;

                                if (block_st + 1 < tokens.size() and
                                    tokens[block_st + 1]->type == TOKEN_KEYWORD)
                                {
                                    tok_keyword *k2 = (tok_keyword *)tokens[block_st + 1];
                                    saw_elif = k2->value == KEYWORD_IF;
                                }

                                if (saw_elif)
                                {
                                    do
                                    {
                                        int elif_cond_st = block_st + 2;
                                        int elif_body_st = -1;

                                        gb = 0;
                                        ast_expression *elif_cond = nullptr;

                                        for (size_t j = elif_cond_st; j < tokens.size(); j++)
                                        {
                                            token *t = tokens[j];

                                            if (t->type == TOKEN_OPERATOR)
                                            {
                                                tok_operator *o = (tok_operator *)t;

                                                if (o->value == "(" or
                                                    o->value == "{" or
                                                    o->value == "[")
                                                {
                                                    gb++;
                                                }
                                                else if (o->value == ")" or
                                                         o->value == "}" or
                                                         o->value == "]")
                                                {
                                                    gb--;
                                                }
                                            }

                                            if (t->type == TOKEN_NEWLINE and not gb)
                                            {
                                                elif_body_st = j;
                                                break;
                                            }
                                        }

                                        assert(elif_body_st != -1);
                                        elif_cond = parse_expression(tokens, elif_cond_st, elif_body_st);

                                        std::vector<token *> elif_body = get_block(tokens, elif_body_st, tab_count);
                                        block_st = elif_body_st + elif_body.size();

                                        while (block_st < tokens.size())
                                        {
                                            if (tokens[block_st]->type == TOKEN_NEWLINE or
                                                tokens[block_st]->type == TOKEN_TABSPACE)
                                            {
                                                block_st++;
                                                continue;
                                            }
                                            break;
                                        }

                                        elifs.push_back(std::make_pair(elif_cond, parse_ast(elif_body)));

                                        if (block_st >= tokens.size())
                                            break;

                                        if (tokens[block_st]->type != TOKEN_KEYWORD)
                                            break;

                                        tok_keyword *k = (tok_keyword *)tokens[block_st];

                                        if (k->value != KEYWORD_ELSE)
                                            break;

                                        if (block_st + 1 >= tokens.size())
                                            break;

                                        if (tokens[block_st + 1]->type != TOKEN_KEYWORD)
                                            break;

                                        tok_keyword *k2 = (tok_keyword *)tokens[block_st + 1];

                                        if (k2->value != KEYWORD_IF)
                                            break;

                                    } while (1);

                                    goto el_l1;
                                }
                                else
                                {
                                    else_body = get_block(tokens, block_st + 1, get_tab_count(tokens, block_st));
                                    block_st += else_body.size();
                                }
                            }
                        }
                    }

                    i = block_st;
                    asts.push_back(new ast_if(if_cond, parse_ast(if_body), elifs, parse_ast(else_body)));
                }
                break;

                default:
                    break;
                }
            }
            break;
            case TOKEN_OPERATOR:
            {
                tok_operator *o = (tok_operator *)c;

                if (o->value == "=")
                {
                    int f_jmp = -1;
                    int end_idx = i;
                    int gb = 0;

                    for (size_t j = i + 1; j < tokens.size(); j++)
                    {
                        token *t = tokens[j];

                        if (t->type == TOKEN_OPERATOR)
                        {
                            tok_operator *o = (tok_operator *)t;

                            if (o->value == "(" or
                                o->value == "{" or
                                o->value == "[")
                            {
                                gb++;
                            }
                            else if (o->value == ")" or
                                     o->value == "}" or
                                     o->value == "]")
                            {
                                gb--;
                            }
                            else if (o->value == ";" and gb == 0)
                            {
                                end_idx = j;
                                break;
                            }
                        }

                        if (t->type == TOKEN_NEWLINE and not gb)
                        {
                            end_idx = j;
                            break;
                        }

                        if (j == tokens.size() - 1 and not gb)
                        {
                            end_idx = j + 1;
                            break;
                        }
                    }

                    assert(end_idx != i);
                    f_jmp = end_idx;

                    ast_expression *expr_val = parse_expression(tokens, i + 1, end_idx);
                    end_idx = i;
                    gb = 0;

                    for (int j = i - 1; j >= 0; j--)
                    {
                        token *t = tokens[j];

                        if (t->type == TOKEN_OPERATOR)
                        {
                            tok_operator *o = (tok_operator *)t;

                            if (o->value == "(" or
                                o->value == "{" or
                                o->value == "[")
                            {
                                gb++;
                            }
                            else if (o->value == ")" or
                                     o->value == "}" or
                                     o->value == "]")
                            {
                                gb--;
                            }
                            else if (o->value == ";" and gb == 0)
                            {
                                end_idx = j;
                                break;
                            }
                        }

                        if (t->type == TOKEN_NEWLINE and not gb)
                        {
                            end_idx = j;
                            break;
                        }
                    }

                    assert(end_idx != i);

                    ast_expression *expr_var = parse_expression(tokens, end_idx + 1, i);
                    asts.push_back(new ast_var_decl(expr_var, expr_val));
                    i = f_jmp;
                }
                else if (o->value == "(")
                {
                    ast_expression *callee;
                    std::vector<ast_expression *> args;

                    int cle_end = -1;
                    int gb = 0;

                    for (int j = i - 1; j >= 0; j--)
                    {
                        token *t = tokens[j];

                        if (t->type == TOKEN_OPERATOR)
                        {
                            tok_operator *o = (tok_operator *)t;

                            if (o->value == "(" or
                                o->value == "{" or
                                o->value == "[")
                            {
                                gb++;
                            }
                            else if (o->value == ")" or
                                     o->value == "}" or
                                     o->value == "]")
                            {
                                gb--;
                            }
                            else if (o->value == ";" and gb == 0)
                            {
                                cle_end = j;
                                break;
                            }
                        }

                        if (t->type == TOKEN_NEWLINE and not gb)
                        {
                            cle_end = j;
                            break;
                        }

                        if (not j and not gb)
                        {
                            cle_end = j;
                            break;
                        }
                    }

                    assert(cle_end != -1);
                    callee = parse_expression(tokens, cle_end + 1, i);

                    int arg_x = i + 1;
                    int arg_y = i + 1;
                    gb = 0;

                    for (size_t j = i + 1; j < tokens.size(); j++)
                    {
                        token *t = tokens[j];

                        if (t->type == TOKEN_OPERATOR)
                        {
                            tok_operator *o = (tok_operator *)t;

                            if (o->value == "(" or
                                o->value == "{" or
                                o->value == "[")
                            {
                                gb++;
                            }
                            else if (o->value == ")" or
                                     o->value == "}" or
                                     o->value == "]")
                            {
                                if (o->value == ")" and gb == 0)
                                {
                                    if (tokens[j - 1]->type == TOKEN_OPERATOR)
                                    {
                                        tok_operator *o = (tok_operator *)tokens[j - 1];

                                        if (o->value == "," or o->value == "(")
                                            break;
                                    }

                                    arg_y = j;
                                    args.push_back(parse_expression(tokens, arg_x, arg_y));
                                    arg_x = j + 1;
                                    break;
                                }

                                gb--;
                            }
                            else if (o->value == "," and gb == 0)
                            {
                                arg_y = j;
                                args.push_back(parse_expression(tokens, arg_x, arg_y));
                                arg_x = j + 1;
                            }
                        }
                    }

                    asts.push_back(new ast_func_call(callee, args));
                    i = arg_y;
                }
            }
            break;

            default:
                break;
            }
        }

        return asts;
    }

    SF_API ast_expression *parse_expression(std::vector<token *> tokens, int start, int end)
    {
        ast_expression *expr = nullptr;

        for (size_t i = start; i < end; i++)
        {
            token *c = tokens[i];

            switch (c->type)
            {
            case TOKEN_IDENTIFIER:
            {
                expr = new ast_expr_var(((tok_identifier *)c)->value);
            }
            break;
            case TOKEN_INTEGER:
            {
                expr = new ast_expr_int(((tok_integer *)c)->value);
            }
            break;
            case TOKEN_FLOAT:
            {
                expr = new ast_expr_float(((tok_float *)c)->value);
            }
            break;
            case TOKEN_STRING:
            {
                expr = new ast_expr_string(((tok_string *)c)->value);
            }
            break;
            case TOKEN_KEYWORD:
            {
                tok_keyword *k = (tok_keyword *)c;

                switch (k->type)
                {
                case KEYWORD_TRUE:
                {
                    expr = new ast_expr_bool(true);
                }
                break;
                case KEYWORD_FALSE:
                {
                    expr = new ast_expr_bool(false);
                }
                break;

                default:
                    break;
                }
            }
            break;
            case TOKEN_OPERATOR:
            {
                tok_operator *o = (tok_operator *)c;

                if (o->value == "[")
                {
                    if (expr == nullptr)
                    {
                        /* array declaration */
                        std::vector<stack_node *> args;
                        int gb = 0;

                        int arg_x = i + 1;
                        int arg_y = i + 1;

                        for (size_t j = i + 1; j < tokens.size(); j++)
                        {
                            token *t = tokens[j];

                            if (t->type == TOKEN_OPERATOR)
                            {
                                tok_operator *o = (tok_operator *)t;

                                if (o->value == "(" or
                                    o->value == "{" or
                                    o->value == "[")
                                {
                                    gb++;
                                }
                                else if (o->value == ")" or
                                         o->value == "}" or
                                         o->value == "]")
                                {
                                    if (o->value == "]" and gb == 0)
                                    {
                                        if (tokens[j - 1]->type == TOKEN_OPERATOR)
                                        {
                                            tok_operator *o = (tok_operator *)tokens[j - 1];

                                            if (o->value == ",")
                                                break;
                                        }

                                        arg_y = j;
                                        args.push_back(new stack_node(parse_expression(tokens, arg_x, arg_y), -1));
                                        arg_x = j + 1;
                                        break;
                                    }

                                    gb--;
                                }
                                else if (o->value == "," and gb == 0)
                                {
                                    arg_y = j;
                                    args.push_back(new stack_node(parse_expression(tokens, arg_x, arg_y), -1));
                                    arg_x = j + 1;
                                }
                            }
                        }

                        expr = new ast_expr_array(args);
                        i = arg_y - 1;
                    }
                    else
                    {
                        int gb = 0;
                        int end_square_bracket = -1;

                        for (size_t j = i + 1; j < tokens.size(); j++)
                        {
                            token *t = tokens[j];

                            if (t->type == TOKEN_OPERATOR)
                            {
                                tok_operator *o = (tok_operator *)t;

                                if (o->value == "(" or
                                    o->value == "{" or
                                    o->value == "[")
                                {
                                    gb++;
                                }
                                else if (o->value == ")" or
                                         o->value == "}" or
                                         o->value == "]")
                                {
                                    if (o->value == "]" and gb == 0)
                                    {
                                        end_square_bracket = j;
                                        break;
                                    }

                                    gb--;
                                }
                            }
                        }

                        ast_expression *index = parse_expression(tokens, i + 1, end_square_bracket);
                        expr = new ast_expr_square_access(expr, index);
                        i = end_square_bracket;
                    }
                }
            }
            break;

            default:
                break;
            }
        }

        expr->evaluated = false;
        return expr;
    }

    SF_API void _sf_print_ast(ast *node)
    {
        std::cout << "ast: (" << node->type << "): ";
        switch (node->type)
        {
        case AST_EXPRESSION:
        {
            ast_expression *expr = (ast_expression *)node;

            switch (expr->expr_type)
            {
            case AST_EXPR_VAR:
            {
                ast_expr_var *var = (ast_expr_var *)expr;
                std::cout << "var: " << var->name << std::endl;
            }
            break;

            case AST_EXPR_INT:
            {
                ast_expr_int *i = (ast_expr_int *)expr;
                std::cout << "int: " << i->value << std::endl;
            }
            break;

            case AST_EXPR_FLOAT:
            {
                ast_expr_float *f = (ast_expr_float *)expr;
                std::cout << "float: " << f->value << std::endl;
            }
            break;

            case AST_EXPR_STRING:
            {
                ast_expr_string *s = (ast_expr_string *)expr;
                std::cout << "string: " << s->value << std::endl;
            }
            break;

            case AST_EXPR_BOOL:
            {
                ast_expr_bool *b = (ast_expr_bool *)expr;
                std::cout << "bool: " << b->value << std::endl;
            }
            break;

            case AST_EXPR_FUNCTION:
            {
                ast_expr_function *f = (ast_expr_function *)expr;
                std::cout << "function: " << std::endl;
                std::cout << "args (" << f->value->args.size() << "): \n";
                for (size_t i = 0; i < f->value->args.size(); i++)
                {
                    std::cout << f->value->args[i] << std::endl;
                }

                if (not f->value->isnative)
                {
                    std::cout << "body: \n";
                    _sf_print_ast(f->value->body);
                }
                else
                    std::cout << "isnative: 1\n";
            }
            break;

            case AST_EXPR_ARRAY:
            {
                ast_expr_array *a = (ast_expr_array *)expr;
                std::cout << "array: " << std::endl;
                std::cout << "elements (" << a->value.size() << "): \n";
                for (size_t i = 0; i < a->value.size(); i++)
                {
                    std::cout << "element " << i << ": \n";
                    _sf_print_ast(a->value[i]->val);
                }
            }
            break;

            case AST_EXPR_SQUARE_ACCESS:
            {
                ast_expr_square_access *a = (ast_expr_square_access *)expr;
                std::cout << "square_access: " << std::endl;
                std::cout << "lval: \n";
                _sf_print_ast(a->lval);
                std::cout << "index: \n";
                _sf_print_ast(a->index);
            }
            break;

            default:
                std::cout << "unknown expr: " << expr->expr_type << std::endl;
                break;
            }
        }
        break;

        case AST_VAR_DECL:
        {
            ast_var_decl *decl = (ast_var_decl *)node;
            std::cout << "var_decl: " << std::endl;
            _sf_print_ast(decl->name);
            _sf_print_ast(decl->value);
        }
        break;

        case AST_FUNC_CALL:
        {
            ast_func_call *call = (ast_func_call *)node;
            std::cout << "func_call: " << std::endl;
            _sf_print_ast(call->name);
            std::cout << "args: \n";
            for (size_t i = 0; i < call->args.size(); i++)
            {
                _sf_print_ast(call->args[i]);
            }
        }
        break;

        case AST_IF:
        {
            ast_if *ifnode = (ast_if *)node;
            std::cout << "if: " << std::endl;
            std::cout << "condition: \n";
            assert(ifnode->condition != nullptr);
            _sf_print_ast(ifnode->condition);
            std::cout << "body: \n";
            _sf_print_ast(ifnode->body);
            std::cout << "elif: (" << ifnode->elifs.size() << ")\n";
            for (size_t i = 0; i < ifnode->elifs.size(); i++)
            {
                std::cout << "elif: " << i << std::endl;
                std::cout << "condition: \n";
                _sf_print_ast(ifnode->elifs[i].first);
                std::cout << "body: \n";
                _sf_print_ast(ifnode->elifs[i].second);
            }
            std::cout << "else: (" << ifnode->else_body.size() << ")\n";
            _sf_print_ast(ifnode->else_body);
        }
        break;

        default:
            break;
        }
    }

    SF_API void _sf_print_ast(std::vector<ast *> asts)
    {
        for (size_t i = 0; i < asts.size(); i++)
        {
            _sf_print_ast(asts[i]);
        }
    }

    SF_API int
    get_tab_count(std::vector<token *> toks, int start)
    {
        for (int i = start - 1; i >= 0; i--)
        {
            if (toks[i]->type == TOKEN_TABSPACE)
                return ((tok_tabspace *)toks[i])->value;

            if (toks[i]->type == TOKEN_NEWLINE)
                return 0;
        }

        return 0;
    }

    SF_API std::vector<token *>
    get_block(std::vector<token *> toks, int start, int tab_count)
    {
        std::vector<token *> block;

        for (size_t i = start; i < toks.size(); i++)
        {
            if (toks[i]->type == TOKEN_NEWLINE)
            {
                int j = i;
                while (j < toks.size() and toks[j]->type == TOKEN_NEWLINE)
                    j++;

                if (j >= toks.size())
                    return block;

                if (toks[j]->type == TOKEN_TABSPACE)
                {
                    if (((tok_tabspace *)toks[j])->value <= tab_count)
                        return block;
                }
                else
                    return block;
            }

            block.push_back(toks[i]);
        }

        return block;
    }

    SF_API std::string ast_expr_array::to_string()
    {
        std::string str = "[";
        for (size_t i = 0; i < value.size(); i++)
        {
            str += value[i]->val->to_string();
            if (i != value.size() - 1)
                str += ", ";
        }
        str += "]";
        return str;
    }

    ast_expr_array::~ast_expr_array()
    {
        int lbp = -1;

        for (auto &i : value)
        {
            if (lbp == -1)
                lbp = i->st_idx;
            else if (i->st_idx < lbp)
                lbp = i->st_idx;

            i->ref_count--;
        }
        
        gstack_set_bp(lbp);
        gstack_pop_bp();
    }

    SF_API int 
    get_expr_unique_id(void)
    {
        static int id = 0;
        return id++;
    }
} // namespace sf