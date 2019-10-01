// An implementation of the Lox language using the Pattern library, declarative style
// http://www.craftinginterpreters.com/scanning.html
// http://www.craftinginterpreters.com/representing-code.html
// https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox

#ifndef LOXPROTONICE
#define LOXPROTONICE

#include "pattern2.cpp"
#include "nice-enum.cpp"
#include <string_view>

using std::string_view;


// Pre-defined, should be added to the Pattern Library
constexpr bool is_digit (char c)            { return '0' <= c && c <= '9'; }
constexpr bool is_alpha (char c)            { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
constexpr bool is_alpha_numeric (char c)    { return is_alpha(c) || is_digit(c); }
constexpr bool is_whitespace (char c)       { return c == ' ' || c == '\t' || c == '\r'; }


namespace LoxLang
{

// ---------------------------------------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------------------------------
// Lexing
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

    END
};

#include <tuple>
using namespace std::literals;

// For importing TokenType members
namespace TokenTypeMembers
{
    namespace detail
    {
        // constexpr string_view member_str = "LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, IDENTIFIER, STRING, NUMBER, AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, END"sv;
        constexpr string_view member_str = "LEFT_PAREN, RIGHT_PAREN"sv;

        std::tuple<string_view, string_view> names = PL::split(member_str,
                               [] (char c) constexpr -> bool { return c == ','; },
                               [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; });
        
        constexpr int count = std::tuple_size_v<decltype(names)>;
    }

    const auto [
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

        END] = make_index_tuple<detail::count>();
}

constexpr char* to_cstring (TokenType member)
{
    return std::get<static_cast<int>(member)>( TokenTypeMembers::detail::names );
}




std::string to_string (std::nullptr_t)     { return "nullptr"; }



// namespace Lex {
//     Token number (ParseTree tree)    { return {TokenType::NUMBER, std::stod(joined(tree))}; }
// }


} // LoxLang
#endif