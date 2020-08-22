#ifndef PATTERN_H
#define PATTERN_H


#include "scanner.h"


#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

using namespace PL::Scan;


// Tentative architecture:
// Lexical Analysis:
//     Scanner:    identifies a lexical sequence, advancing a position if a match is found
//     Matcher:    structures a lexical sequence, returns a syntax tree
//     Lexer:      assigns meaning to a lexical sequence, returns a token
// Syntax Analysis:
//     Pattern:    identifies a semantic sequence, returns a sequence of tokens
//     Parser:     structures a semantic sequence, returns an abstract syntax tree
// Semantic Analysis:
//     Translator: assigns meaning to a semantic sequence, returns a block of code in a target language

// Guidelines:
// End goal is for syntax to be used for user-defined interfaces. Usable interfaces are not compatible with bugs; if the
// system introduces new opportunities for bugs, it won't be adopted by the mainstream programming community. Current
// state of the art indicates language definition is riddled with the opportunity to introduce bugs -- because grammars
// are turning complete, writing a grammar is equivalent to writing a program. Ambiguity is unavoidable, but cannot be
// tolerated.

// Seemingly the only solution to this issue is to introduce structured grammar facilities. In this way, only well-
// behaved notations will be created. Full expressiveness (using the same underlying system for inter-compatibility) can
// be implemented in a separate, dangerous library.

// Patterns are one good solution to introduce structured notations. Lisp-style macros are another. Patterns must be
// carefully curated, like they are in Haskell, to avoid any ambiguities. Macros may be superior in this way, and I need
// to explore this issue.

// Patterns may be superior in the sense that they can define template patterns that adapt to the localized needs of the
// syntax, even adjusting in ambiguous situations to automatically choose alternate syntaxes. As an example, a
// programmer should choose "list-pattern" over "bracketed-comma-separated-list", so that if the user imports another
// library with the same syntax, one library can be made to use brackets and the other braces, within a local lexical
// scope. Better yet, so a user can import a css-like file to customize the syntax.

// Ideally, a large set of syntactical patterns could be devised covering all major use cases in a non-ambiguous way.
// Then, programmers could mix and match as needed, without feeling deprived. This should include error messages -
// standard messages, and standardized message reporting utilities.


namespace PatDef
{

// ---------------------------------------------------------------------------------------------------------------------
// Pre-defined predicates
// ---------------------------------------------------------------------------------------------------------------------
constexpr bool is_any_char     (char c)     { return true; }
constexpr bool is_lower        (char c)     { return 'a' <= c && c <= 'z'; }
constexpr bool is_upper        (char c)     { return 'A' <= c && c <= 'Z'; }
constexpr bool is_digit        (char c)     { return '0' <= c && c <= '9'; }
constexpr bool is_ascii_symbol (char c)
{
     return ('!' <= c && c <= '/') ||     // !"#$%&'()*+,-./
            (':' <= c && c <= '@') ||     // :;<=>?@
            ('[' <= c && c <= '`') ||     // [\]^_`
            ('{' <= c && c <= '~');       // {|}~
}

constexpr bool is_octal_digit  (char c)     { return '0' <= c && c <= '7'; }
constexpr bool is_binary_digit (char c)     { return c == '0' || c == '1'; }
constexpr bool is_hex_lower    (char c)     { return is_digit(c) || ('a' <= c && c <= 'f'); }
constexpr bool is_hex_upper    (char c)     { return is_digit(c) || ('A' <= c && c <= 'F'); }
constexpr bool is_hex_digit    (char c)     { return is_hex_lower(c) || ('A' <= c && c <= 'F'); }
constexpr bool is_letter       (char c)     { return is_lower(c) || is_upper(c); }
constexpr bool is_alphanumeric (char c)     { return is_letter(c) || is_digit(c); }
constexpr bool is_whitespace   (char c)     { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
constexpr bool is_comma        (char c)     { return c == ','; }


// ---------------------------------------------------------------------------------------------------------------------
// Abstractions
// ---------------------------------------------------------------------------------------------------------------------
namespace Detail
{

     template <std::size_t... I>
     bool any_in_string_impl (char c, const char* string, std::index_sequence<I...>)
     {
          return (... || c == string[I]);
     }

     template <std::size_t N>
     bool any_in_string (char c, const char string[N])
     {
          return any_in_string_impl(c, string, std::make_index_sequence<N>{});
     }

}


// Pattern sugar
template <typename Expr1, typename Expr2>
auto from_to (Expr1 e1, Expr2 e2)     { return join(e1, until(e2)); }


template <typename Expr1, typename Expr2>
auto from_upto (Expr1 e1, Expr2 e2)     { return join(e1, while_not(e2)); }


template <int N>
auto charset (const char (&string)[N])
{
     return PatLib::Scan::lit(Detail::any_in_string<N>, string);
}


template <typename Expr1, typename Expr2>
auto escaped_until (Expr1 end, Expr2 escape)
{
     auto special = any(end, escape);

     return join(while_not(special),
                 many(join(escape, opt(special), while_not(special))),
                 end);
     // compare to advance_past_if_found(), maybe one should replace the other
}


template <typename Expr, typename FirstExpr, typename... RestExpr>
auto sequence (FirstExpr first, RestExpr... rest,
               Expr is_whitespace = is_whitespace)
{
     return join(first, join(is_whitespace, rest)...);
}


template <typename Expr, typename Expr2>
auto sequence_of (Expr e, Expr2 is_whitespace = is_whitespace)
{
     return join(e, many(join(is_whitespace, e)));
}


template <typename Expr1, typename Expr2, typename Expr3>
auto after_delimiter (Expr1 e,
                      Expr2 is_delimiter = is_comma,
                      Expr3 is_whitespace = is_whitespace)
{
     return join(opt(is_whitespace), is_delimiter, opt(is_whitespace), e);
}


template <typename Expr1, typename Expr2, typename FirstExpr, typename... RestExpr>
auto delimited_sequence (FirstExpr first, RestExpr... rest,
                         Expr1 is_delimiter = is_comma,
                         Expr2 is_whitespace = is_whitespace)
{
     return join(first, after_delimiter(rest, is_delimiter, is_whitespace)...);
}


template <typename Expr1, typename Expr2, typename Expr3>
auto delimited_sequence_of (Expr1 e, Expr2 is_delimiter = is_comma, Expr3 is_whitespace = is_whitespace)
{
     return join(e, many(after_delimiter(e, is_delimiter, is_whitespace)));
}


// ---------------------------------------------------------------------------------------------------------------------
// Pre-defined patterns
// ---------------------------------------------------------------------------------------------------------------------
// Character Sets
auto lowercase     = charset("abcdefghijklmnopqrstuvwxyz");
auto uppercase     = charset("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
auto digit         = charset("0123456789");
auto binary_digit  = charset("01");
auto octal_digit   = charset("01234567");
auto hex_lowercase = charset("0123456789abcdef");
auto hex_uppercase = charset("0123456789ABCDEF");
auto hex_digit     = charset("0123456789abcdefABCDEF");
auto ascii_symbol  = charset("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");


// Named patterns
auto letter       = any(lowercase, uppercase);
auto alphanumeric = any(letter, digit);
auto digits       = at_least(1, digit);

auto whitespace = at_least(1, any(' ', '\t', '\r', '\n'));
auto spaces     = at_least(1, ' ');
auto tabs       = at_least(1, '\t');
auto newline    = any("\r\n", '\n');


// Symbolic Encodings
auto integer     = join(charset("123456789"), many(digit));
auto decimal     = join(integer, '.', digits);
auto decimal_alt = any(decimal,
                       join(integer, '.'),
                       join('.', digits));

auto binary            = at_least(1, binary_digit);
auto octal             = at_least(1, octal_digit);
auto hexadecimal_lower = at_least(1, hex_lowercase);
auto hexadecimal_upper = at_least(1, hex_uppercase);
auto hexadecimal       = at_least(1, hex_digit);


// Compound patterns
template <typename Expr>
auto line_comment (Expr start)     { return from_upto(start, newline); }


template <typename Expr>
auto string_double (Expr escape = '\\')     { return join('"', escaped_until('"', escape)); }


template <typename Expr>
auto string_single (Expr escape = '\\')     { return join("'", escaped_until("'", escape)); }


template <typename Expr>
auto string (Expr escape = '\\')     { return any(string_single(escape), string_double(escape)); }


// indent
// block_comment(open, close)
// group(open, middle, close)
// list
// block
// binary operation
// function application


} // namespace PatDef


#endif // PATTERN_H
