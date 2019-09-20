// An implementation of the Lox language using the Pattern library
// http://www.craftinginterpreters.com/scanning.html
// http://www.craftinginterpreters.com/representing-code.html
// https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox

#ifndef LOXPROTOTEST
#define LOXPROTOTEST

#include "pattern2.cpp"

#include <fstream>    // get_file_contents
#include <iostream>
#include <map>
#include <memory>     // unique_ptr
#include <string>
#include <vector>

using std::string_view;



// ---------------------------------------------------------------------------------------------------------------------
// Test
// ---------------------------------------------------------------------------------------------------------------------
namespace LoxLang
{

using namespace Scan;

constexpr bool is_digit (char c)            { return '0' <= c && c <= '9'; }
constexpr bool is_alpha (char c)            { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
constexpr bool is_alpha_numeric (char c)    { return is_alpha(c) || is_digit(c); }
constexpr bool is_whitespace (char c)       { return c == ' ' || c == '\r' || c == '\t'; }

scanner digit         = with(is_digit);
scanner digits        = scan_while(is_digit);
scanner alpha         = with(is_alpha);
scanner alpha_num     = with(is_alpha_numeric);
scanner alpha_nums    = scan_while(is_alpha_numeric);
scanner comment       = join(lit("//"), stop_before('\n'));
scanner whitespace    = with(is_whitespace);
scanner number        = join(digits, lit('.'), digits);
scanner identifier    = join(alpha, alphanums);
scanner string        = join(lit('"'), until('"'));


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

std::string to_string (std::string val)    { return val; }
std::string to_string (std::nullptr_t)     { return "nullptr"; }


struct TokenBase {
    virtual std::string to_string () = 0;
};


template <typename ValueType>
struct Token : TokenBase {
    Token (TokenType type, std::string lexeme, ValueType literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line)
    {}

    std::string to_string () override {
        return ::to_string(type) + " " + lexeme + " " + ::to_string(literal);
    }

private:
    const TokenType   type;
    const std::string lexeme;
    const ValueType   literal;
    const int         line;
};



class Scanner
{
public:
    Scanner (const std::string source);
    std::vector<std::unique_ptr<TokenBase>>& scan_tokens ();


private:
    const std::string source;
    string_view view;
    int line = 1;

    std::vector<std::unique_ptr<TokenBase>> tokens;
    const std::map<std::string, TokenType> keywords;

    // Traversal
    inline char peek ()                     { return ::peek(view); }
    inline char peek_next ()                { return ::peek2(view); }
    inline void advance ()                  { if (peek() == '\n') ++line; ::advance(view); }
    inline char get ()                      { char c = peek(); advance(); return c; }
    inline void backtrack ()                { ::backtrack(view); }
    inline bool is_at_end ()                { return view.empty(); }
    inline bool advance_if (scanner&& s)    { return ::advance_if(view, std::forward<scanner>(s)); }

    // Recognizers
    bool match (char expected)    { return scan_with(view, expected); }

    // Tokenizers
    bool identifier ();
    bool number ();
    bool string ();

    // Run
    void scan_token ();
    void add_token (TokenType type);

    template <typename ValueType>
    void add_token (TokenType type, ValueType literal);
};


bool Scanner::identifier ()
{
    auto i = Match::with(identifier);
    if (!i)    return false;

    auto match = keywords.find(*i);
    if (match == keywords.end())    add_token(TokenType::IDENTIFIER, *i);
    else                            add_token(match->second);
    
    return true;
}


bool Scanner::string ()
{
    auto s = Match::with(string);
    if (!s)    return false;

    s->remove_prefix(1);
    s->remove_suffix(1);
    add_token(TokenType::STRING, *s);
    return true;
}


bool Scanner::number ()
{
    auto n = Match::with(number);
    if (!n)    return false;

    add_token(TokenType::NUMBER, std::stod(*n));
    return true;
}


void Scanner::scan_token ()
{
    char c = get();

    switch (c)
    {
        using namespace TokenTypeMembers;

        case '(' : add_token(LEFT_PAREN);  break;
        case ')' : add_token(RIGHT_PAREN); break;
        case '{' : add_token(LEFT_BRACE);  break;
        case '}' : add_token(RIGHT_BRACE); break;
        case ',' : add_token(COMMA);       break;
        case '.' : add_token(DOT);         break;
        case '-' : add_token(MINUS);       break;
        case '+' : add_token(PLUS);        break;
        case ';' : add_token(SEMICOLON);   break;
        case '*' : add_token(STAR);        break;
        case '!' : add_token(match('=') ? BANG_EQUAL    : BANG);    break;
        case '=' : add_token(match('=') ? EQUAL_EQUAL   : EQUAL);   break;
        case '<' : add_token(match('=') ? LESS_EQUAL    : LESS);    break;
        case '>' : add_token(match('=') ? GREATER_EQUAL : GREATER); break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            break;
        default :
            backtrack();

                 if (advance_if(comment))    break;
            else if (match('/'))             { add_token(SLASH); break; }
            else if (string())               break;
            else if (number())               break;
            else if (identifier())           break;
            else                             error("Unexpected character.");
    };
}


} // namespace LoxLang

#endif    // LOXPROTOTEST