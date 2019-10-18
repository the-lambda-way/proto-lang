// An implementation of the Lox language in C++
// http://www.craftinginterpreters.com

#ifndef LOXLOWLEVEL
#define LOXLOWLEVEL

#include <iostream>    // cout, getline
#include <map>         // keywords
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>      // token list
#include "../include/scan_view.h"
#include "../include/scanning-algorithms.h"
#include "../include/syntax.h"
#include "lox-common.h"

using std::string_view;
using std::tuple;
using namespace std::string_literals;
using namespace std::string_view_literals;



// ---------------------------------------------------------------------------------------------------------------------
//  Tokenizers
// ---------------------------------------------------------------------------------------------------------------------
constexpr bool is_digit (char c)            { return '0' <= c && c <= '9'; }
constexpr bool is_alpha (char c)            { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
constexpr bool is_alpha_numeric (char c)    { return is_alpha(c) || is_digit(c); }
constexpr bool is_whitespace (char c)       { return c == ' ' || c == '\t' || c == '\r'; }


lox_token identifier (scan_view& s)
{
    while (is_alpha_numeric(*s))    ++s;

    // See if the identifier is a reserved word.
    std::string match = to_string(s.skipped());

    auto keyword = keywords.find(match);

    if   (keyword == keywords.end())    return {TokenType::IDENTIFIER, match, s.skipped()};
    else                                return {keyword->second, empty, s.skipped()};
}


lox_token number (scan_view& s)
{
    while (is_digit(*s))    ++s;

    if (*s == '.' && is_digit(s[1]))
    {
        s += 2;
        while (is_digit(*s))    ++s;
    }

    double val = std::stod(to_string(s.skipped()));
    return {TokenType::NUMBER, val, s.skipped()};
}


lox_token string (scan_view& s)
{
    while (*s != '"' && s.has_more())    ++s;

    if (s.eof())    return {TokenType::ERROR, "Unterminated string."s, s.skipped()};
    ++s;

    return {TokenType::STRING, s.skipped(1, 2), s.skipped()};
}


// ---------------------------------------------------------------------------------------------------------------------
//  Scanner
// ---------------------------------------------------------------------------------------------------------------------
std::vector<lox_token> scan_tokens (const std::string& source) {
    std::vector<lox_token> tokens;
    scan_view s {source};

    while (!s.eof())
    {
        s.save();
        char c = *s++;

        switch (c)
        {
            using namespace TokenTypeMembers;

            case '(' : tokens.emplace_back(LEFT_PAREN,  empty, s.skipped()); break;
            case ')' : tokens.emplace_back(RIGHT_PAREN, empty, s.skipped()); break;
            case '{' : tokens.emplace_back(LEFT_BRACE,  empty, s.skipped()); break;
            case '}' : tokens.emplace_back(RIGHT_BRACE, empty, s.skipped()); break;
            case ',' : tokens.emplace_back(COMMA,       empty, s.skipped()); break;
            case '.' : tokens.emplace_back(DOT,         empty, s.skipped()); break;
            case '-' : tokens.emplace_back(MINUS,       empty, s.skipped()); break;
            case '+' : tokens.emplace_back(PLUS,        empty, s.skipped()); break;
            case ';' : tokens.emplace_back(SEMICOLON,   empty, s.skipped()); break;
            case '*' : tokens.emplace_back(STAR,        empty, s.skipped()); break;
            case '!' : *s == '=' ? tokens.emplace_back(BANG_EQUAL,    empty, (++s).skipped())
                                 : tokens.emplace_back(BANG,          empty, s.skipped());
                       break;
            case '=' : *s == '=' ? tokens.emplace_back(EQUAL_EQUAL,   empty, (++s).skipped())
                                 : tokens.emplace_back(EQUAL,         empty, s.skipped());
                       break;
            case '<' : *s == '=' ? tokens.emplace_back(LESS_EQUAL,    empty, (++s).skipped())
                                 : tokens.emplace_back(LESS,          empty, s.skipped());
                       break;
            case '>' : *s == '=' ? tokens.emplace_back(GREATER_EQUAL, empty, (++s).skipped())
                                 : tokens.emplace_back(GREATER,       empty,  s.skipped());
                       break;

            case '/' :
                if (*s == '/')
                    // A comment goes until the end of the line.
                    while (*s != '\n' && !s.eof())    ++s;
                else
                    tokens.emplace_back(SLASH, empty, s.skipped());
                break;

            case ' '  :
            case '\r' :
            case '\t' :
            case '\n' : break;    // Ignore whitespace.

            case '"' : tokens.emplace_back(string(s));
                       break;

            default :
                if      (is_digit(c))    tokens.emplace_back(number(s));
                else if (is_alpha(c))    tokens.emplace_back(identifier(s));
                else                     tokens.emplace_back(ERROR, "Unexpected character: "s, s.skipped());
                break;
        }
    }

    tokens.emplace_back(TokenType::END, empty, s.skipped());
    return tokens;
}


int main (int argc, char* argv[]) {
    return lox_main(argc, argv);
}


#endif    // LOXLOWLEVEL
