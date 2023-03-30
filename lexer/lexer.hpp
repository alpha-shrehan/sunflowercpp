#pragma once

#include "../header.hpp"
#include "token.hpp"

namespace sf
{
    std::vector<token*> tokenize(std::string source);
    const char *keyword_type_to_string(KeywordEnum type);
} // namespace sf
