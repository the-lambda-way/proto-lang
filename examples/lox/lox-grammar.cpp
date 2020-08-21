// An implementation of the Lox language using grammar abstractions
// http://www.craftinginterpreters.com

#pragma once

#include <string_view>
#include "../../pattern.h"
#include "lox-common.h"

using namespace std::string_view_literals;
using namespace Pattern;


namespace LoxScan {

using namespace PatLib::Scan;

auto identifier     = letter + *alphanumeric);
auto number         = digits + ~('.'_p + digits);
auto partial_string = '"'_p + while_not('"');
auto line_comment   = "//"_p + while_not(newline);


// ---------------------------------------------------------------------------------------------------------------------
// Better?
auto identifier     = grammar("_ _*", letter, alphanumeric);
auto number         = grammar("_1 (. _1)?", digits);
auto partial_string = grammar("\" (!\")*");
auto line_comment   = grammar("// (!_)*", newline);

auto digit   = grammar("0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9");
auto integer = grammar("_+", digit);
auto decimal = grammar("_1 . _1", integer);

// ---------------------------------------------------------------------------------------------------------------------
// suppose I provide a way to register custom rule identifiers
// this probably wouldn't work unless I use type erasure to give the patterns a type, so they can be forward declared
// perhaps this can be a function pointer type without loss of flexibility
// see https://www.reddit.com/r/cpp/comments/bhxx49/c20_string_literals_as_nontype_template/
pattern letter, alphanumeric, digit, digits, newline;
#define REGISTER_PATTERN(name)     void visit_grammar<"name"> (scan_view& s) { name(s); }
REGISTER_PATTERN(letter)
REGISTER_PATTERN(alphanumeric)
REGISTER_PATTERN(digit)
REGISTER_PATTERN(digits)
REGISTER_PATTERN(newline)

pattern identifier     = "letter alphanumeric*"_p;
pattern number         = "digits (. digits)?"_p;
pattern partial_string = "\" (!\")*"_p;
pattern line_comment   = "// (!newline)*"_p;;

pattern digit   = "0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9"_p;
pattern integer = "digit+"_p;
pattern decimal = "integer . integer"_p;

// ---------------------------------------------------------------------------------------------------------------------

} // namespace LoxScan


class LoxLexer
{
public:
     LoxLexer (std::string_view source) : s {source} {}

     bool has_more ()     { return s.has_more(); }

     lox_token next ()
     {
          advance_while(s, fn::any(whitespace, line_comment));

          s.save();

          return choice(symbol, string, number, identifier, unknown);
     }

private:
     scan_view s;

     lox_token make_token (TokenType type, lox_token_value value = empty)
     {
          return {type, value, s.skipped()};
     }

     std::optional<lox_token> symbol ()
     {
          using namespace TokenTypeMembers;

          switch (*s)
          {
               // single symbols
               case '(' :     ++s; return make_token(LEFT_PAREN);
               case ')' :     ++s; return make_token(RIGHT_PAREN);
               case '{' :     ++s; return make_token(LEFT_BRACE);
               case '}' :     ++s; return make_token(RIGHT_BRACE);
               case ',' :     ++s; return make_token(COMMA);
               case '.' :     ++s; return make_token(DOT);
               case '-' :     ++s; return make_token(MINUS);
               case '+' :     ++s; return make_token(PLUS);
               case ';' :     ++s; return make_token(SEMICOLON);
               case '*' :     ++s; return make_token(STAR);
               case '/' :     ++s; return make_token(SLASH);

               // double symbols
               case '!' :     ++s; return make_token(lit('=') ? BANG_EQUAL    : BANG);
               case '=' :     ++s; return make_token(lit('=') ? EQUAL_EQUAL   : EQUAL);
               case '<' :     ++s; return make_token(lit('=') ? LESS_EQUAL    : LESS);
               case '>' :     ++s; return make_token(lit('=') ? GREATER_EQUAL : GREATER);

               default  :     return {};
          }
     }

     std::optional<lox_token> string ()
     {
          if (!LoxScan::partial_string(s))     return {};

          if (s.eof())     return make_token(TokenType::ERROR, "Unterminated string."sv);

          ++s;
          return make_token(TokenType::STRING, s.skipped(0, 2));
     }

     std::optional<lox_token> number ()
     {
          if (!LoxScan::number(s))     return {};
          return make_token(TokenType::NUMBER, std::stod(s.copy_skipped()));
     }

     std::optional<lox_token> identifier ()
     {
          if (!LoxScan::identifier(s))     return {};

          std::string_view match = s.skipped();

          auto keyword = keywords.find(match);

          if (keyword != keywords.end())     return make_token(keyword->second);
          else                               return make_token(TokenType::IDENTIFIER, match);
     }

     std::optional<lox_token> unknown ()
     {
          return make_token(TokenType::ERROR, "Unexpected character: "sv);
     }

}; // class LoxLexer



int main (int argc, char* argv[])
{
    return lox_main(argc, argv);
}
