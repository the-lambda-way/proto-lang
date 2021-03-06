// An implementation of the Lox language using scanning algorithms
// http://www.craftinginterpreters.com

#pragma once

#include <string_view>
#include "../../pattern/scan_view.h"
#include "../../pattern/scanning-algorithms.h"
#include "../../pattern/pattern.h"
#include "lox-common.h"

using namespace std::string_view_literals;
using namespace Pattern;


class LoxLexer
{
public:
     LoxLexer (std::string_view source) : s {source} {}

     bool has_more ()     { return s.has_more(); }

     lox_token next ()
     {
          s.save();
          char c = *s++;

          switch (c)
          {
               using namespace TokenTypeMembers;

               // single symbols
               case '('  :     return make_token(LEFT_PAREN);
               case ')'  :     return make_token(RIGHT_PAREN);
               case '{'  :     return make_token(LEFT_BRACE);
               case '}'  :     return make_token(RIGHT_BRACE);
               case ','  :     return make_token(COMMA);
               case '.'  :     return make_token(DOT);
               case '-'  :     return make_token(MINUS);
               case '+'  :     return make_token(PLUS);
               case ';'  :     return make_token(SEMICOLON);
               case '*'  :     return make_token(STAR);

               case ' '  :
               case '\r' :
               case '\t' :
               case '\n' :     break;    // Ignore whitespace


               // double symbols
               case '!'  :     return make_token(match('=') ? BANG_EQUAL    : BANG);
               case '='  :     return make_token(match('=') ? EQUAL_EAUAL   : EQUAL);
               case '<'  :     return make_token(match('=') ? LESS_EQUAL    : LESS);
               case '>'  :     return make_token(match('=') ? GREATER_EQUAL : GREATER);

               case '/'  :
                    if (*s != '/')     return make_token(SLASH);

                    advance_while_not(s, '\n');     // line comment
                    return next_token();


               // larger tokens
               case '"' :      return string();

               default  :
                    if      (is_digit(c))      return number();
                    else if (is_letter(c))     return identifier();
                    else                       return make_token(ERROR, "Unexpected character: "sv);
          }
     }


private:
     scan_view s;

     lox_token make_token (TokenType type, lox_token_value value = empty)
     {
          return {type, value, s.skipped()};
     }

     bool match (char expected)
     {
          if (*s != expected)     return false;

          ++s;
          return true;
     }

     lox_token identifier ()
     {
          advance_while(s, is_alphanumeric);
          std::string_view match = s.skipped();

          auto keyword = keywords.find(match);

          if (keyword != keywords.end())     return make_token(keyword->second);
          else                               return make_token(TokenType::IDENTIFIER, match);
     }

     lox_token number ()
     {
          advance_while(s, is_digit);

          if (*s == '.' && is_digit(s[1]))
          {
               s += 2;
               advance_while(s, is_digit);
          }

          return make_token(TokenType::NUMBER, std::stod(s.copy_skipped()));
     }

     lox_token string ()
     {
          advance_while_not(s, '"');

          if (s.eof())     return make_token(TokenType::ERROR, "Unterminated string."sv);

          ++s;
          return make_token(TokenType::STRING, s.skipped(0, 2));
     }

}; // class LoxLexer


int main (int argc, char* argv[])
{
    return lox_main(argc, argv);
}

