#pragma once

#include <iostream>
#include <map>          // keywords
#include <string>
#include <string_view>
#include <variant>      // token values
#include "../../pattern.h"

using namespace Pattern;


enum class TokenType
{
     // Single-character tokens
     LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
     COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

     // One or two character tokens
     BANG, BANG_EQUAL,
     EQUAL, EQUAL_EQUAL,
     GREATER, GREATER_EQUAL,
     LESS, LESS_EQUAL,

     // Literals
     IDENTIFIER, STRING, NUMBER,

     // Keywords
     AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
     PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

     END, NONE, ERROR
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
     constexpr auto NONE          = TokenType::NONE;
     constexpr auto ERROR         = TokenType::ERROR;
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


using lox_token_value = variant<std::monostate,    // valueless tokens
                                string_view,       // identifier, string, error
                                double>;           // number

using lox_token = token_lex<TokenType, lox_token_value>;
auto empty = std::monostate {};


std::string to_string (TokenType type)
{
     static const std::string strings[] =
     {
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

std::string to_string (const std::monostate& s)    { return "";                     }
std::string to_string (const std::string& s)       { return s;                      }
std::string to_string (std::string_view s)         { return {s.data(), s.length()}; }
std::string to_string (double d)                   { return std::to_string(d);      }
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
     std::string val;

     switch (t.tag)
     {
          case (TokenType::IDENTIFIER) :
          case (TokenType::STRING)     :
          case (TokenType::NUMBER)     : val = to_string(t.value); break;
          default                      : val = to_string(t.tag);
     }

     if (t.tag == TokenType::ERROR)     val += " " + to_string(t.lexeme);

     return std::format("{}\t{:15} : {}", to_string(t.source_location(data)), to_string(t.tag), val);
}


// ---------------------------------------------------------------------------------------------------------------------
//  Infrastructure
// ---------------------------------------------------------------------------------------------------------------------
class System
{
public:
     bool had_error = false;

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
extern class LoxLexer;


void run (std::string_view source)
{
     LoxLexer lox {source};

     while (lox.has_more())
          std::cout << to_string(lox.next(), source.data()) << '\n';
}


void run_file (std::string_view path)
{
     const std::string code = file_to_string(path);
     run(code);

     if (lox_system.had_error)    exit(EXIT_FAILURE);
}


void run_prompt ()
{
     std::string buf;

     for (;;) {
          std::cout << "> ";
          getline(std::cin, buf);
          run(buf);

          if (lox_system.had_error)    exit(EXIT_FAILURE);
     }
}


int lox_main (int argc, char* argv[])
{
     try
     {
          if      (argc >  2)     std::cout << "Usage: lox [script]";
          else if (argc == 2)     run_file(argv[1]);
          else                    run_prompt();
     }
     catch (std::exception& e) {
          std::cerr << e.what();
     }
}
