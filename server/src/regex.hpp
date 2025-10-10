#pragma once
#include <string>
#include <regex>

std::string globToRegex(const std::string &pattern)
{
    std::string regex;
    regex += '^';
    for (char c : pattern)
    {
        switch (c)
        {
        case '*':
            regex += ".*";
            break;
        case '?':
            regex += ".";
            break;
        case '.':
            regex += "\\.";
            break;
        case '\\':
            regex += "\\\\";
            break;
        default:
            regex += c;
            break;
        }
    }
    regex += "$";
    return regex;
}