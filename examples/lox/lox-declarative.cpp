// An implementation of the Lox language using declarative abstractions
// http://www.craftinginterpreters.com

#pragma once

#include <string_view>
#include "../../pattern.h"
#include "lox-common.h"

using namespace std::string_view_literals;



// Still not happy with this. Seems to have more cognitive overload than should be necessary (disjoint abstractions).
// Perhaps syntax-macros would be clearer than this configuration.



namespace LoxScan {

using namespace PatLib::Scan;

auto identifier     = join(any(letter, '_'), many(any(alphanumeric, '_')));
auto number         = any(decimal, integer);
auto partial_string = join('"', while_not('"'));
auto comment        = join("//", while_not(newline));
auto nontoken       = at_least(1, any(whitespace, newline, comment));

} // namespace LoxScan


class LoxLexer
{
public:
     LoxLex (std::string_view source) : scanner {source} {}

     bool      has_more   ()     { return scanner.has_more(); }
     lox_token next_token ()     { non_token(scanner); return lexer(scanner); }

private:
     scan_view scanner;

     tokenizer_t lexer = tokenize_with<lox_token>(
          double_symbols,
          symbols,
          { LoxScan::partial_string, string },
          { LoxScan::number, number },
          { LoxScan::identifier, identifier },
          { PatDef::otherwise, unknown }
     )

     inline lox_token make_token (TokenType tag, lox_token_value value = empty)
     {
          return {tag, value, scanner.skipped()};
     }

     std::optional<lox_token> double_symbols (scan_view& s)
     {
          using namespace TokenTypeMembers;

          if (scan(s, "!="))     return make_token(BANG_EQUAL);
          if (scan(s, "=="))     return make_token(EQUAL_EQUAL);
          if (scan(s, "<="))     return make_token(LESS_EQUAL);
          if (scan(s, ">="))     return make_token(GREATER_EQUAL);

          return {};
     }

     std::optional<lox_token> symbols (scan_view& s)
     {
          using namespace TokenTypeMembers;

          switch (*s)
          {
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
               case '!' :     ++s; return make_token(BANG);
               case '<' :     ++s; return make_token(LESS);
               case '>' :     ++s; return make_token(GREATER);
               case '/' :     ++s; return make_token(SLASH);
          }

          return {};
     }

     lox_token string (parse_tree match)
     {
          auto [_, contents] = match;

          if (s.eof())    return {TokenType::ERROR, "Unterminated string."sv, match.lexeme()};

          ++s;
          return {TokenType::STRING, contents, match.lexeme()};
     }

     lox_token number (parse_tree match)
     {
          return make_token(TokenType::NUMBER, std::stod(to_string(match.lexeme())));
     }

     lox_token identifier (parse_tree match)
     {
          auto keyword = keywords.find(match);

          if (keyword != keywords.end())     return {keyword->second, empty, match.lexeme()};
          else                               return make_token(TokenType::IDENTIFIER, match.lexeme());
     }

     lox_token unknown (parse_tree match)
     {
          return {TokenType::ERROR, "Unexpected character: "sv, match.lexeme()};
     }

}; // class LoxLexer


int main (int argc, char* argv[])
{
     return lox_main(argc, argv);
}

