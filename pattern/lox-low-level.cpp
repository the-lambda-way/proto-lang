// An implementation of the Lox language in C++
// http://www.craftinginterpreters.com

#ifndef LOXLOWLEVEL
#define LOXLOWLEVEL

#include <iostream>    // cout, getline
#include <string>
#include <string_view>
#include <vector>      // token list
#include "../include/scan_view.h"
#include "../include/syntax.h"
#include "../include/scanning-algorithms.h"
#include "lox-common.h"

using std::string_view;
using namespace std::string_literals;
using namespace std::string_view_literals;



// ---------------------------------------------------------------------------------------------------------------------
//  Tokenizers
// ---------------------------------------------------------------------------------------------------------------------
lox_token identifier (scan_view& s)
{
    while (is_alphanumeric(*s))    ++s;
    string_view match = s.skipped();

    auto keyword = keywords.find(match);

    if (keyword != keywords.end())    return {keyword->second, empty, match};
    else                              return {TokenType::IDENTIFIER, match, match};
}


lox_token number (scan_view& s)
{
    while (is_digit(*s))    ++s;

    if (*s == '.' && is_digit(s[1]))
    {
        s += 2;
        while (is_digit(*s))    ++s;
    }

    double val = std::stod(s.copy_skipped());
    return {TokenType::NUMBER, val, s.skipped()};
}


lox_token string (scan_view& s)
{
    while (*s != '"' && s.has_more())    ++s;

    if (s.eof())    return {TokenType::ERROR, "Unterminated string."s, s.skipped()};
    ++s;

    return {TokenType::STRING, s.skipped(1, 1), s.skipped()};
}


// ---------------------------------------------------------------------------------------------------------------------
//  Scanner
// ---------------------------------------------------------------------------------------------------------------------
void next_token (std::vector<lox_token>& tokens, scan_view& s)
{
    s.save();
    char c = *s++;

    switch (c)
    {
        using namespace TokenTypeMembers;

        // single symbols
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

        case ' '  :
        case '\r' :
        case '\t' :
        case '\n' : break;    // Ignore whitespace


        // double symbols
        case '!' : *s == '=' ? tokens.emplace_back(BANG_EQUAL,    empty, (++s).skipped())
                             : tokens.emplace_back(BANG,          empty,     s.skipped());
                   break;
        case '=' : *s == '=' ? tokens.emplace_back(EQUAL_EQUAL,   empty, (++s).skipped())
                             : tokens.emplace_back(EQUAL,         empty,     s.skipped());
                   break;
        case '<' : *s == '=' ? tokens.emplace_back(LESS_EQUAL,    empty, (++s).skipped())
                             : tokens.emplace_back(LESS,          empty,     s.skipped());
                   break;
        case '>' : *s == '=' ? tokens.emplace_back(GREATER_EQUAL, empty, (++s).skipped())
                             : tokens.emplace_back(GREATER,       empty,     s.skipped());
                   break;

        case '/' :
            if (*s == '/')
                // A comment goes until the end of the line.
                while (*s != '\n' && !s.eof())    ++s;
            else
                tokens.emplace_back(SLASH, empty, s.skipped());
            break;


        // larger tokens
        case '"' : tokens.push_back(string(s));
                   break;

        default :
            if      (is_digit(c))     tokens.push_back(number(s));
            else if (is_letter(c))    tokens.push_back(identifier(s));
            else                      tokens.emplace_back(ERROR, "Unexpected character: "s, s.skipped());

    } // switch
}


std::vector<lox_token> scan_tokens (const std::string& source)
{
    std::vector<lox_token> tokens;
    scan_view s {source};

    while (s.has_more())    next_token(tokens, s);

    tokens.emplace_back(TokenType::END, empty);
    return tokens;
}


int main (int argc, char* argv[]) {
    return lox_main(argc, argv);
}


#endif    // LOXLOWLEVEL
