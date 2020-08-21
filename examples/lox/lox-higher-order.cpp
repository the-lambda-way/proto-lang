// An implementation of the Lox language using higher-order functions
// http://www.craftinginterpreters.com

#pragma once

#include <string_view>
#include "../../pattern.h"
#include "lox-common.h"

using namespace std::string_view_literals;
using namespace Pattern;


namespace LoxScan {

using namespace PatLib::Scan;

auto identifier     = join(letter, many(alphanumeric));
auto number         = join(digits, opt(join('.', digits)));
auto partial_string = join('"', while_not('"'));
auto comment_end    = join('/', while_not(newline));

}


class LoxLexer
{
public:
     LoxLexer (std::string_view source) : s {source} {}

     bool has_more ()     { return s.has_more(); }

     lox_token next ()
     {
          using namespace TokenTypeMembers;

          s.save();

          switch (*s++)
          {
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
               case '/'  :     return *s == '/' ? skip_comment(++s) : make_token(SLASH);


               // larger tokens
               default  :
                    if (LoxScan::partial_string(s))     return string();
                    if (LoxScan::number(s))             return make_token(NUMBER, std::stod(s.copy_skipped()));
                    if (LoxScan::identifier(s))         return identifier();

                    return make_token(ERROR, "Unexpected character: "sv);
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

     lox_token skip_comment ()
     {
          LoxScan::comment_end(s);
          return next_token();
     }

     lox_token identifier ()
     {
          std::string_view match = s.skipped();

          auto keyword = keywords.find(match);

          if (keyword != keywords.end())     return make_token(keyword->second);
          else                               return make_token(TokenType::IDENTIFIER, match);
     }

     lox_token string ()
     {
          if (s.eof())     return make_token(TokenType::ERROR, "Unterminated string."sv);

          ++s;
          return make_token(TokenType::STRING, s.skipped(0, 2));
     }

}; // class LoxLexer


int main (int argc, char* argv[])
{
    return lox_main(argc, argv);
}

