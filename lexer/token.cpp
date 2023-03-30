#include "token.hpp"
#include "lexer.hpp"

namespace sf
{
    token::token(LexerTokenEnum type, int line, int column)
    {
        this->type = type;
        this->line = line;
        this->column = column;
    }

    SF_API void
    _sf_token_print(token *tok)
    {
        if (tok == nullptr)
            throw std::runtime_error("Token is null");

        std::cout << "Token [Line: " << tok->line << ", Column: " << tok->column << ", Type: " << tok->type << "] := ";
        switch (tok->type)
        {
        case TOKEN_EOF:
            std::cout << "EOF" << std::endl;
            break;
        case TOKEN_IDENTIFIER:
            std::cout << "Identifier: " << ((tok_identifier *)tok)->value << std::endl;
            break;
        case TOKEN_STRING:
            std::cout << "String: " << ((tok_string *)tok)->value << std::endl;
            break;
        case TOKEN_INTEGER:
            std::cout << "Integer: " << ((tok_integer *)tok)->value << std::endl;
            break;
        case TOKEN_FLOAT:
            std::cout << "Float: " << ((tok_float *)tok)->value << std::endl;
            break;
        case TOKEN_OPERATOR:
            std::cout << "Operator: " << ((tok_operator *)tok)->value << std::endl;
            break;
        case TOKEN_NEWLINE:
            std::cout << "Newline" << std::endl;
            break;
        case TOKEN_TABSPACE:
            std::cout << "Tabspace: " << ((tok_tabspace *)tok)->value << std::endl;
            break;
        case TOKEN_KEYWORD:
            std::cout << "Keyword: " << keyword_type_to_string(((tok_keyword *)tok)->value) << std::endl;
            break;

        default:
            std::cout << "Unknown" << std::endl;
            break;
        }
    }

    SF_API void
    _sf_token_print(std::vector<token *> tokens)
    {
        for (int i = 0; i < tokens.size(); i++)
        {
            _sf_token_print(tokens[i]);
        }
    }
} // namespace sf
