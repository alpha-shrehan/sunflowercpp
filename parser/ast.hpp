#pragma once

#include "../header.hpp"
#include "../lexer/token.hpp"
#include "sffunc.hpp"

enum ASTEnum
{
    AST_NULL = -1, /* THIS IS NOT A LANGUAGE AST */
    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_FUNC_CALL,
    AST_RETURN,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_EXPRESSION,
};

enum ASTExprEnum
{
    AST_EXPR_NULL = -1, /* THIS IS NOT A LANGUAGE EXPR */
    AST_EXPR_VAR,
    AST_EXPR_FUNC_CALL,
    AST_EXPR_INT,
    AST_EXPR_FLOAT,
    AST_EXPR_STRING,
    AST_EXPR_BOOL,
    AST_EXPR_FUNCTION,
    AST_EXPR_ARRAY,
    AST_EXPR_SQUARE_ACCESS /* This is used for accessing an array's element */
};

namespace sf
{
    SF_API int get_expr_unique_id(void);
    class stack_node;
    class SF_API ast
    {
    public:
        ASTEnum type;
        bool evaluated = false;
        ast(ASTEnum type) : type(type){};
        ast() : type(AST_NULL){};
    };

    class SF_API ast_expression : public ast
    {
    public:
        int id;
        bool anonymous = true; /* not in stack */
        ASTExprEnum expr_type;
        ast_expression(ASTExprEnum expr_type) : ast(AST_EXPRESSION), expr_type(expr_type){ id = get_expr_unique_id(); };
        ast_expression() : ast(AST_EXPRESSION){ id = get_expr_unique_id(); };

        virtual std::string to_string() { return "This expression cannot be printed to stdout."; };
        virtual ~ast_expression(){}; /* This is needed to prevent memory leaks */
    };

    SF_API std::vector<ast *> parse_ast(std::vector<token *> tokens);
    SF_API ast_expression *parse_expression(std::vector<token *> tokens, int start, int end);
    SF_API void _sf_print_ast(ast *node);
    SF_API void _sf_print_ast(std::vector<ast *> nodes);
    SF_API bool expr_to_cbool(ast_expression *expr); /* This is defined in ip.cpp */

    SF_API int get_tab_count(std::vector<token *>, int start);
    SF_API std::vector<token *> get_block(std::vector<token *> toks, int start, int tab_count);

    class SF_API ast_expr_var : public ast_expression
    {
    public:
        std::string name;
        ast_expr_var(std::string name) : ast_expression(AST_EXPR_VAR), name(name){};
    };

    class SF_API ast_expr_func_call : public ast_expression
    {
    public:
        std::string name;
        std::vector<ast_expression *> args;
        ast_expr_func_call(std::string name, std::vector<ast_expression *> args) : ast_expression(AST_EXPR_FUNC_CALL), name(name), args(args){};
    };

    class SF_API ast_expr_int : public ast_expression
    {
    public:
        int value;
        ast_expr_int(int value) : ast_expression(AST_EXPR_INT), value(value){};

        std::string to_string() { return std::to_string(value); };
    };

    class SF_API ast_expr_float : public ast_expression
    {
    public:
        float value;
        ast_expr_float(float value) : ast_expression(AST_EXPR_FLOAT), value(value){};

        std::string to_string() { return std::to_string(value); };
    };

    class SF_API ast_expr_string : public ast_expression
    {
    public:
        std::string value;
        ast_expr_string(std::string value) : ast_expression(AST_EXPR_STRING), value(value){};

        std::string to_string() { return value; };
    };

    class SF_API ast_expr_bool : public ast_expression
    {
    public:
        bool value;
        ast_expr_bool(bool value) : ast_expression(AST_EXPR_BOOL), value(value){};

        std::string to_string() { return value ? "True" : "False"; };
    };

    class SF_API ast_expr_function : public ast_expression
    {
    public:
        function *value;
        ast_expr_function(function *value) : ast_expression(AST_EXPR_FUNCTION), value(value){};

        std::string to_string() { return "function '" + value->name + "'"; };
    };

    class SF_API ast_expr_array : public ast_expression
    {
    public:
        std::vector<stack_node *> value;
        ast_expr_array(std::vector<stack_node *> value) : ast_expression(AST_EXPR_ARRAY), value(value){};

        std::string to_string();
        ~ast_expr_array();
    };

    class SF_API ast_expr_square_access : public ast_expression
    {
    public:
        ast_expression *lval;
        ast_expression *index;
        ast_expr_square_access(ast_expression *lval, ast_expression *index) : ast_expression(AST_EXPR_SQUARE_ACCESS), lval(lval), index(index){};

        std::string to_string() { return lval->to_string() + "[" + index->to_string() + "]"; };
    };

    class SF_API ast_var_decl : public ast
    {
    public:
        ast_expression *name;
        ast_expression *value;
        ast_var_decl(ast_expression *name, ast_expression *value) : ast(AST_VAR_DECL), name(name), value(value){};
    };

    class SF_API ast_func_decl : public ast
    {
    public:
        std::string name;
        std::vector<std::string> args;
        std::vector<ast *> body;
        ast_func_decl(std::string name, std::vector<std::string> args, std::vector<ast *> body) : ast(AST_FUNC_DECL), name(name), args(args), body(body){};
    };

    class SF_API ast_func_call : public ast
    {
    public:
        ast_expression *name;
        std::vector<ast_expression *> args;
        ast_func_call(ast_expression *name, std::vector<ast_expression *> args) : ast(AST_FUNC_CALL), name(name), args(args){};
    };

    class SF_API ast_return : public ast
    {
    public:
        ast_expression *value;
        ast_return(ast_expression *value) : ast(AST_RETURN), value(value){};
    };

    class SF_API ast_if : public ast
    {
    public:
        ast_expression *condition;
        std::vector<ast *> body;
        std::vector<std::pair<ast_expression *, std::vector<ast *>>> elifs;
        std::vector<ast *> else_body;
        ast_if(
            ast_expression *condition,
            std::vector<ast *> body,
            std::vector<
                std::pair<
                    ast_expression *,
                    std::vector<ast *>>>
                elifs,
            std::vector<ast *> else_body)
            : ast(AST_IF),
              condition(condition),
              body(body),
              elifs(elifs),
              else_body(else_body){};
    };

    class SF_API ast_while : public ast
    {
    public:
        ast_expression *condition;
        std::vector<ast *> body;
        ast_while(ast_expression *condition, std::vector<ast *> body) : ast(AST_WHILE), condition(condition), body(body){};
    };
} // namespace sf