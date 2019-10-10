// An implementation of the Lox language using the Pattern library, nicer style
// http://www.craftinginterpreters.com/scanning.html
// http://www.craftinginterpreters.com/representing-code.html
// https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox

#ifndef LOXPROTONICE
#define LOXPROTONICE

#include <iostream>                 // cout
#include <map>                      // keywords
#include <string_view>
#include <tuple>                    // token container
#include "scanner.h"
#include "../include/scanning-algorithms.h"
#include "../include/syntax.h"

using std::string_view;
using std::tuple;


// Pre-defined, should be added to the Pattern Library
constexpr bool is_digit (char c)            { return '0' <= c && c <= '9'; }
constexpr bool is_alpha (char c)            { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
constexpr bool is_alpha_numeric (char c)    { return is_alpha(c) || is_digit(c); }
constexpr bool is_whitespace (char c)       { return c == ' ' || c == '\t' || c == '\r'; }


namespace LoxLang
{

// ---------------------------------------------------------------------------------------------------------------------
//  Terminals
// ---------------------------------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------------------------------
// Token type
// ---------------------------------------------------------------------------------------------------------------------
// NICE_ENUM(TokenType, LEFT_PAREN, RIGHT_PAREN)
    // Single-character tokens.
    // LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE
    // COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // // One or two character tokens.
    // BANG, BANG_EQUAL,
    // EQUAL, EQUAL_EQUAL,
    // GREATER, GREATER_EQUAL,
    // LESS, LESS_EQUAL,

    // // Literals.
    // IDENTIFIER, STRING, NUMBER,

    // // Keywords.
    // AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    // PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    // END
// )

enum class TokenType
{
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END, NONE
};

// For importing TokenType members
namespace TokenTypeMembers
{
    constexpr auto LEFT_PAREN    = TokenType::LEFT_PAREN;
    constexpr auto RIGHT_PAREN   = TokenType::RIGHT_PAREN;
    constexpr auto LEFT_BRACE    = TokenType::LEFT_BRACE;
    constexpr auto RIGHT_BRACE   = TokenType::RIGHT_BRACE;
    constexpr auto COMMA         = TokenType::COMMA;
    constexpr auto DOT           = TokenType::DOT;
    constexpr auto MINUS         = TokenType::MINUS;
    constexpr auto PLUS          = TokenType::PLUS;
    constexpr auto SEMICOLON     = TokenType::SEMICOLON;
    constexpr auto SLASH         = TokenType::SLASH;
    constexpr auto STAR          = TokenType::STAR;
    constexpr auto BANG          = TokenType::BANG;
    constexpr auto BANG_EQUAL    = TokenType::BANG_EQUAL;
    constexpr auto EQUAL         = TokenType::EQUAL;
    constexpr auto EQUAL_EQUAL   = TokenType::EQUAL_EQUAL;
    constexpr auto GREATER       = TokenType::GREATER;
    constexpr auto GREATER_EQUAL = TokenType::GREATER_EQUAL;
    constexpr auto LESS          = TokenType::LESS;
    constexpr auto LESS_EQUAL    = TokenType::LESS_EQUAL;
    constexpr auto IDENTIFIER    = TokenType::IDENTIFIER;
    constexpr auto STRING        = TokenType::STRING;
    constexpr auto NUMBER        = TokenType::NUMBER;
    constexpr auto AND           = TokenType::AND;
    constexpr auto CLASS         = TokenType::CLASS;
    constexpr auto ELSE          = TokenType::ELSE;
    constexpr auto FALSE         = TokenType::FALSE;
    constexpr auto FUN           = TokenType::FUN;
    constexpr auto FOR           = TokenType::FOR;
    constexpr auto IF            = TokenType::IF;
    constexpr auto NIL           = TokenType::NIL;
    constexpr auto OR            = TokenType::OR;
    constexpr auto PRINT         = TokenType::PRINT;
    constexpr auto RETURN        = TokenType::RETURN;
    constexpr auto SUPER         = TokenType::SUPER;
    constexpr auto THIS          = TokenType::THIS;
    constexpr auto TRUE          = TokenType::TRUE;
    constexpr auto VAR           = TokenType::VAR;
    constexpr auto WHILE         = TokenType::WHILE;
    constexpr auto END           = TokenType::END;
}


std::string to_string (TokenType type) {
    const std::string strings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
        "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",
        "IDENTIFIER", "STRING", "NUMBER",
        "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
        "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
        "END"
    };

    return strings[static_cast<int>(type)];
}

std::string to_string (std::nullptr_t)     { return "nullptr"; }


const std::map<std::string, TokenType> keywords =
    {
        { "and",    TokenType::AND    },
        { "class",  TokenType::CLASS  },
        { "else",   TokenType::ELSE   },
        { "false",  TokenType::FALSE  },
        { "for",    TokenType::FOR    },
        { "fun",    TokenType::FUN    },
        { "if",     TokenType::IF     },
        { "nil",    TokenType::NIL    },
        { "or",     TokenType::OR     },
        { "print",  TokenType::PRINT  },
        { "return", TokenType::RETURN },
        { "super",  TokenType::SUPER  },
        { "this",   TokenType::THIS   },
        { "true",   TokenType::TRUE   },
        { "var",    TokenType::VAR    },
        { "while",  TokenType::WHILE  },
    };


template <typename ValueType>
using lox_token = token_loc<TokenType, ValueType>;


template <typename T, typename V>
std::string to_string (token_loc<T, V> t)
{
    return to_string(t.type) + " " + t.location.to_string() + " " + to_string(t.literal);
}


// ---------------------------------------------------------------------------------------------------------------------
//  Tokens
// ---------------------------------------------------------------------------------------------------------------------
lox_token<void> none_token {TokenType::NONE, nullptr};


// This can be made nicer
template <typename V>
lox_token<V> identifier (scanner& s)
{
    s.save();
    if (!(advance_if(s, is_alpha) && advance_while(s, is_alpha_numeric))    return none_token;
    string_view match = s.skipped();

    auto kw_match = keywords.find(match.substr);
    if (kw_match == keywords.end())    return {TokenType::IDENTIFIER, match, s.location()};
    else                               return {kw_match->second, match, s.location()};
}

// void Scanner::identifier ()
// {
//     string_view match = match_when(view, LoxLang::identifier).value();

//     auto kw_match = keywords.find(to_string(match));
//     if (kw_match == keywords.end())    add_token(TokenType::IDENTIFIER, match, match);
//     else                               add_token(kw_match->second, match);
// }

// ---------------------------------------------------------------------------------------------------------------------
//  Engine
// ---------------------------------------------------------------------------------------------------------------------
class System
{
public:
    void report (file_position pos, std::string where, std::string message)
    {
        std::cout << "[at " << pos.line << ":" << pos.column << "] Error " << where << ": " << message;
        had_error = true;
    }


    void error (file_position pos, std::string message)
    {
        report(pos, "", message);
    }


    template <typename T>
    void error (T token, std::string message)
    {
        report(token.position(), " at '" + token.lexeme() + "'", message);
    }

private:
    bool had_error         = false;
    bool had_runtime_error = false;
};


template <typename... TokenList>
tuple<TokenList...>& tokenize (string_view source)
{

}




} // LoxLang


#endif // LOXPROTONICE
