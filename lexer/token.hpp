#pragma once

#include "../header.hpp"

enum LexerTokenEnum
{
    TOKEN_NULL = -1, /* THIS IS NOT A LANGUAGE TOKEN */
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,
    TOKEN_TABSPACE, /* Tabspace and space are alike terms */
    TOKEN_NEWLINE,
};

enum KeywordEnum
{
    KEYWORD_NULL = -1, /* THIS IS NOT A LANGUAGE KEYWORD */
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_RETURN,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
};

namespace sf
{
    class SF_API token
    {
    public:
        LexerTokenEnum type;
        int line;
        int column;
        token(LexerTokenEnum type, int line, int column);
        token(): type(TOKEN_NULL) { line = column = -1; };
    };

    class SF_API tok_identifier: public token
    {
    public:
        std::string value;
        tok_identifier(std::string value, int line, int column): token(TOKEN_IDENTIFIER, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_string: public token
    {
    public:
        std::string value;
        tok_string(std::string value, int line, int column): token(TOKEN_STRING, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_integer: public token
    {
    public:
        int value;
        tok_integer(int value, int line, int column): token(TOKEN_INTEGER, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_float: public token
    {
    public:
        float value;
        tok_float(float value, int line, int column): token(TOKEN_FLOAT, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_operator: public token
    {
    public:
        std::string value;
        tok_operator(std::string value, int line, int column): token(TOKEN_OPERATOR, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_keyword: public token
    {
    public:
        KeywordEnum value;
        tok_keyword(KeywordEnum value, int line, int column): token(TOKEN_KEYWORD, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_comment: public token
    {
    public:
        std::string value;
        tok_comment(std::string value, int line, int column): token(TOKEN_COMMENT, line, column)
        {
            this->value = value;
        }
    };

    class SF_API tok_tabspace: public token
    {
    public:
        int value;
        tok_tabspace(int value, int line, int column): token(TOKEN_TABSPACE, line, column)
        {
            this->value = value;
        }
    };

    SF_API void _sf_token_print(token *tok);
    SF_API void _sf_token_print(std::vector<token *> tokens);

} // namespace sf
