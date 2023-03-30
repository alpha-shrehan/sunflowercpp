#include "lexer.hpp"

struct
{
    KeywordEnum i;
    const char *j;
} KEYWORDS[] = {
    {KEYWORD_IF, "if"},
    {KEYWORD_ELSE, "else"},
    {KEYWORD_WHILE, "while"},
    {KEYWORD_FOR, "for"},
    {KEYWORD_BREAK, "break"},
    {KEYWORD_CONTINUE, "continue"},
    {KEYWORD_RETURN, "return"},
    {KEYWORD_TRUE, "True"},
    {KEYWORD_FALSE, "False"},
    {KEYWORD_NULL, NULL}
};

namespace sf
{
    bool _sf_is_operator(char c)
    {
        return std::string("+-*/%&|!^~<>[](){}:,.\\=").find(c) != std::string::npos;
    }

    std::string _sf_make_operator(char c1, char c2, char c3)
    {
        std::string op;

        switch (c1)
        {
        case '+':
        case '-':
        case '/':
        case '%':
        case '&':
        case '|':
        case '!':
        case '^':
        case '~':
        case '=':
        {
            op += c1;
            if (c2 == '=')
                op += c2;
        }
        break;
        case '<':
        case '>':
        case '*':
        {
            op += c1;
            if (c2 == '=')
                op += c2;
            else if (c2 == c1)
                op += c2;

            if (c3 == '=')
                op += c3;
        }
        break;
        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case ':':
        case ',':
        case '.':
        case '\\':
        {
            op += c1;
        }
        break;

        default:
            break;
        }

        return op;
    }

    int _sf_keyword_type(std::string str)
    {
        for (int i = 0; KEYWORDS[i].j != NULL; i++)
        {
            if (str == KEYWORDS[i].j)
                return KEYWORDS[i].i;
        }

        return KEYWORD_NULL;
    }

    std::vector<token *> tokenize(std::string source)
    {
        std::vector<token *> tokens;
        int line = 1;
        int column = 1;

        for (int i = 0; i < source.size(); i++)
        {
            char c = source[i];

            if (_sf_is_operator(c))
            {
                char c1 = c, c2, c3;
                c2 = c3 = 0;

                if (i + 2 < source.size())
                {
                    c2 = source[i + 1];
                    c3 = source[i + 2];
                }
                else if (i + 1 < source.size())
                    c2 = source[i + 1];
                
                std::string op = _sf_make_operator(c1, c2, c3);
                tokens.push_back(new tok_operator(op, line, column));
                i += op.size() - 1;
                column += op.size();

                continue;
            }
            else if (std::isalpha(c) || c == '_')
            {
                std::string str;
                str += c;

                for (int j = i + 1; j < source.size(); j++)
                {
                    if (std::isalnum(source[j]) || source[j] == '_')
                        str += source[j];
                    else
                        break;
                }

                i += str.size() - 1;
                int kw_type = _sf_keyword_type(str);

                if (kw_type != KEYWORD_NULL)
                    tokens.push_back(new tok_keyword(KeywordEnum(kw_type), line, column));
                else
                    tokens.push_back(new tok_identifier(str, line, column));
                
                column += str.size();
                continue;
            }

            switch (c)
            {
            case '\n':
            {
                tokens.push_back(new token(TOKEN_NEWLINE, line, column));

                line++;
                column = 1;
            }
            break;
            case ' ':
            {
                if (tokens.size())
                {
                    if (tokens.back()->type == TOKEN_TABSPACE)
                    {
                        ((tok_tabspace *)tokens.back())->value++;
                        column++;
                    }
                    else
                        goto L1;
                }
                else
                {
                L1:
                    if (i and source[i - 1] == ' ')
                    {
                        tokens.push_back(new tok_tabspace(2, line, column));
                        column++;
                    }
                }
            }
            break;
            case '\t':
            {
                if (tokens.size())
                {
                    if (tokens.back()->type == TOKEN_TABSPACE)
                    {
                        ((tok_tabspace *)tokens.back())->value += 4;
                        column += 3;
                    }
                    else
                        goto L2;
                }
                else
                {
                L2:
                    tokens.push_back(new tok_tabspace(4, line, column));
                    column += 3;
                }
            }
            break;
            case '\'':
            case '\"':
            {
                std::string value;
                int start = i;
                char end = c;

                while (true)
                {
                    i++;

                    if (i >= source.size())
                        throw std::runtime_error("Unexpected end of file");

                    c = source[i];

                    if (c == end)
                        break;

                    value += c;
                }

                tokens.push_back(new tok_string(value, line, column));
                column += i - start;
            }
            break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                std::string value;
                int start = i;
                bool saw_dot = false;

                while (true)
                {
                    if (i >= source.size())
                        break;

                    c = source[i];

                    if (c == '.')
                    {
                        if (saw_dot)
                            break;

                        saw_dot = true;
                        value += c;
                        i++;
                        continue;
                    }

                    if (c < '0' or c > '9')
                        break;

                    value += c;
                    i++;
                }

                if (saw_dot)
                    tokens.push_back(new tok_float(std::stof(value), line, column));
                else
                    tokens.push_back(new tok_integer(std::stoi(value), line, column));

                column += i - start;
                i--;
            }
            break;

            default:
                throw std::runtime_error("Unknown token");
                break;
            }
        }

        tokens.push_back(new token(TOKEN_EOF, line, column));
        return tokens;
    }

    const char *
    keyword_type_to_string(KeywordEnum type)
    {
        for (int i = 0; KEYWORDS[i].j != NULL; i++)
        {
            if (KEYWORDS[i].i == type)
                return KEYWORDS[i].j;
        }

        return NULL;
    }

} // namespace sf