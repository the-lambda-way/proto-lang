#ifndef LOXCOMMON
#define LOXCOMMON


#include <map>          // keywords
#include <string>
#include <string_view>
#include <variant>      // token values
#include "../include/syntax.h"

using std::string_view;
using std::variant;



constexpr bool is_digit        (char c)    { return '0' <= c && c <= '9'; }
constexpr bool is_letter       (char c)    { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
constexpr bool is_alphanumeric (char c)    { return is_letter(c) || is_digit(c); }
constexpr bool is_whitespace   (char c)    { return c == ' ' || c == '\t' || c == '\r' || '\n'; }


enum class TokenType {
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

    END, NONE, ERROR
};

// For importing TokenType members
namespace TokenTypeMembers {
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
    constexpr auto NONE          = TokenType::NONE;
    constexpr auto ERROR         = TokenType::ERROR;
}

std::string to_string (TokenType type) {
    static const std::string strings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
        "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",
        "IDENTIFIER", "STRING", "NUMBER",
        "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
        "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
        "END", "NONE", "ERROR"
    };

    return strings[static_cast<int>(type)];
}

const std::map<string_view, TokenType> keywords
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

constexpr int TOKEN_STRING_PAD_LENGTH = 15;

using lox_token_value = variant<std::monostate,    // valueless tokens
                                std::string,       // error token
                                string_view,       // identifier, string
                                double>;           // number

using lox_token = token_lex<TokenType, lox_token_value>;
auto empty = std::monostate {};


void pad_right (std::string& s, int amount)
{
    amount -= s.length() > amount ? amount : s.length();
    s.append(amount, ' ');
}

std::string to_string (const std::monostate& s)     { return "";                     }
std::string to_string (const std::string& s)        { return s;                      }
std::string to_string (std::string_view s)          { return {s.data(), s.length()}; }
std::string to_string (double d)                    { return std::to_string(d);      }
std::string to_string (const source_location s)
{
    return "[" + std::to_string(s.line) + ", " + std::to_string(s.column) + "]";
}

std::string to_string (const lox_token_value& v)
{
    return std::visit([] (auto&& arg) { return to_string(arg); }, v);
}


template <typename CharT>
std::string to_string (const lox_token& t, const CharT* data)
{
    std::string msg = to_string(t.tag);
    pad_right(msg, TOKEN_STRING_PAD_LENGTH);

    msg = to_string(t.source_location(data)) + "\t  " + msg + " : "
         + (std::holds_alternative<std::monostate>(t.value)
            ? to_string(t.tag)
            : t.tag == TokenType::STRING
                ? to_string(t.lexeme)
                : to_string(t.value)
        );

    if (t.tag == TokenType::ERROR)    msg += " " + to_string(t.lexeme);

    return msg;
}


// ---------------------------------------------------------------------------------------------------------------------
//  Infrastructure
// ---------------------------------------------------------------------------------------------------------------------
class System
{
public:
    bool had_error         = false;
    bool had_runtime_error = false;

    void report (source_location s, string_view lexeme, std::string message)
    {
        std::cout << "[at " << s.line << ":" << s.column << "] Error " << lexeme << ": " << message;
        had_error = true;
    }


    void error (source_location s, std::string message)
    {
        report(s, "", message);
    }


    void error (scan_view s, std::string message)
    {
        report({s.basis(), s.data()}, "", message);
    }


    void error (scan_view s, string_view lexeme, std::string message)
    {
        report({s.basis(), s.data()}, lexeme, message);
    }


    void error (source_location s, string_view lexeme, std::string message)
    {
        report(s, lexeme, message);
    }
} lox_system;


// ---------------------------------------------------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------------------------------------------------
extern std::vector<lox_token> scan_tokens (const std::string&);


void run (const std::string& source)
{
    std::vector<lox_token> tokens = scan_tokens(source);

    for (const auto& token : tokens) {
        std::cout << to_string(token, source.data()) << "\n";
    }
}


void run_file (std::string path)
{
    const std::string code = file_to_string(path);
    run(code);

    if (lox_system.had_error)            exit(EXIT_FAILURE);
    if (lox_system.had_runtime_error)    exit(EXIT_FAILURE);
  }


void run_prompt ()
{
    std::string line;

    for (;;) {
        std::cout << "> ";
        getline(std::cin, line);
        run(line);

        lox_system.had_error = false;
    }
}


int lox_main (int argc, char* argv[]) {
    using namespace std;

    try {
             if (argc >  2)    cout << "Usage: lox [script]";
        else if (argc == 2)    run_file(argv[1]);
        else                   run_prompt();
    }
    catch (exception& e) {
        cerr << e.what();
    }

    return 0;
}


#endif // LOXCOMMON
