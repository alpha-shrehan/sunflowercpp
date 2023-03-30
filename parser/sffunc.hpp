#pragma once

#include "../header.hpp"

namespace sf
{
    class ast;
    class ast_expression;
    class Module;
    class stack_node;

    class SF_API function
    {
    public:
        std::string name;
        std::vector<std::string> args;
        std::vector<ast_expression *> def_vals;
        std::vector<ast *> body;
        bool isnative;
        bool va_args = false;
        ast_expression *(*native)(Module *m, int *state);
        function(std::string name, std::vector<std::string> args):
            name(name), args(args) {};
    };

    SF_API std::vector<function *> getFunctions();
    SF_API function *add_function_to_fstack(function *f);
} // namespace sf
