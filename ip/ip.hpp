#pragma once

#include "../header.hpp"
#include "name_lookup.hpp"
#include "module.hpp"

enum StateFlagEnum
{
    STATE_OK = 0,
    STATE_ERROR,
    STATE_ABORT,
};

namespace sf
{
    SF_API ast_expression *ip_eval_expr(ast_expression *, Module *);
    SF_API void ip_eval_tree(Module *, int *);

    SF_API void ip_eval_rule_vardecl(ast_var_decl *, Module *, int *);
    SF_API void ip_eval_rule_funcall(ast_func_call *, Module *, int *);
    SF_API void ip_eval_rule_stmt_if(ast_if *, Module *, int *);
} // namespace sf
