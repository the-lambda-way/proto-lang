#ifndef PATTERN2_H
#define PATTERN2_H

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <functional>     // std::search searchers
#include <iostream>       // cout
#include <string>
#include <string_view>
#include <tuple>          // ParseTree
#include <type_traits>    // Destructurable concept
#include <utility>        // std::pair, std::index_sequence

// #include <range/v3/all.hpp>

// using namespace ranges;
using std::forward;
using std::make_tuple;
using std::move;
using std::string_view;
using std::tuple;


// This library relies on a large amount of parameter passing. Use these guidlines to determine how to pass the parameters.
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#fcall-parameter-passing
// Pointers, references, and small values, always take by value, unless forwarding.
// Unknown values, take by reference to const
// Use before move, take by value.
// Forwarding values, take by template parameter &&.



/***********************************************************************************************************************
* Pattern
* The Pattern library provides functions that recognize a pattern beginning at a position of a string.
* 
***********************************************************************************************************************/
// ---------------------------------------------------------------------------------------------------------------------
// Data Types
// ---------------------------------------------------------------------------------------------------------------------

// Ideal return value by function composition
// auto example_nested_tree =
//     std::make_tuple(std::make_tuple("function",
//                                     "add3",
//                                     std::make_tuple("(",
//                                                     std::make_tuple(std::make_tuple("int", "1"),
//                                                                     std::make_tuple("int", "2"),
//                                                                     std::make_tuple("int", "3")),
//                                                     ")")),
//     ";");


// ---------------------------------------------------------------------------------------------------------------------
//  Syntactic sugar
// ---------------------------------------------------------------------------------------------------------------------
using char_predicate = bool (*) (char);

template <typename... Args>
using basic_scanner = bool (*) (string_view, Args...);

using scanner = basic_scanner<>;

template <typename Result, typename... Args>
using basic_matcher = std::optional<Result> (*) (string_view, Args...);

template <typename Result>
using matcher = basic_matcher<Result>;


constexpr int len (char c)              { return 1; }
constexpr int len (char_predicate p)    { return 1; }
constexpr int len (string_view& s)      { return s.length(); }

constexpr char peek (string_view& source)                       { return source.empty() ? '\0' : source.front(); }
constexpr char peek2 (string_view& source)                      { return len(source) <= 1 ? '\0' : source[1]; }
constexpr void advance (string_view& source, int amount = 1)    { source.remove_prefix(amount); }

// Warning: this is more expensive than making an extra copy of source
constexpr void backtrack (string_view& source, int amount = 1)
{
    auto char_ptr = &*(source.begin() - amount);
    source = string_view(char_ptr, std::distance(char_ptr, source.end()));
}


constexpr int difference (string_view first, string_view second)
{
    return std::distance(first.begin(), second.begin());
}


constexpr string_view skipped (string_view source, string_view copy)
{
    return source.substr(0, difference(source, copy));
}


// ---------------------------------------------------------------------------------------------------------------------
// Scanning algorithms
// ---------------------------------------------------------------------------------------------------------------------
// Scanning algorithms should update the position of their source upon a successful match. This reduces the effort of
//     updating the source manually, and allows easy chaining of functions.
// Scanning failure does not guarantee the source was not modified, so that intermediate copies don't need to be made.
// Because of this, complete patterns should make sure to restore the original source position on failure.

// --------------------------------------------------
// Predicates
// --------------------------------------------------
constexpr bool starts_with (string_view source, char c)              { return peek(source) == c; }
constexpr bool starts_with (string_view source, char_predicate p)    { return p(peek(source)); }

constexpr bool starts_with (string_view source, string_view literal)
{
    // fast checks
    if (peek(source) != peek(literal))    return false;    // usual result
    if (len(source) < len(literal))       return false;    // save a check every loop

    // check the rest
    for (int i = 1;    i != len(literal);    ++i)
        if (source[i] != literal[i])    return false;

    return true;
}


// --------------------------------------------------
// Scan beginning
// --------------------------------------------------
template <typename T>
constexpr bool scan_with (string_view& source, T&& t)
{
    int length = len(forward<T>(t));
    if (!starts_with(source, forward<T>(t)))    return false;
    advance(source, length);
    return true;
}


template <typename... Args>
constexpr bool scan_with (string_view& source, basic_scanner<Args...> s, Args&&... args)
{
    string_view copy = source;
    if (!s(copy, forward<Args>(args)...))    return false;

    source = copy;
    return true;
}


template <typename String_View, typename Pred, typename... Args>
constexpr bool advance_if (string_view& source, Pred&& p, Args&&... args)
{
    return scan_with(forward<string_view>(source), forward<Pred>(p), forward<Args>(args)...);
}


template <typename Pred, typename... Args>
constexpr bool scan_optional (string_view& source, Pred&& p, Args&&... args)
{
    scan_with(source, forward<Pred>(p), forward<Args>(args)...);
    return true;
}


// --------------------------------------------------
// Extended scans
// --------------------------------------------------
template <typename Pred, typename... Args>
constexpr bool scan_while (string_view& source, Pred p, const Args&... args)
{
    if (!scan_with(source, p, args...))    return false;
    while (scan_with(source, p, move(args)...));
    return true;
}


template <typename T>
constexpr bool scan_while_not (string_view& source, T t)
{
    int length = len(t);
    while (starts_with(source, t))    advance(source, length);
    return !source.empty();
}


template <typename T>
constexpr bool scan_while_not (string_view& source, string_view literal)
{
    const T searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto [first, _]  = searcher(source.begin(), source.end());

    if (first == source.end())    return false;

    source.remove_prefix(std::distance(source.begin(), first));
    return true;
}


template <typename T>
constexpr bool scan_until (string_view& source, T&& t)
{
    int length = len(forward<T>(t));
    if (!scan_while_not(source, forward<T>(t)))    return false;
    advance(source, length);
    return true;
}


// --------------------------------------------------
// Scan combiners
// --------------------------------------------------
template <typename... Scanners>
constexpr bool scan_join (string_view& source, const Scanners&... s)
{
    return (... && s(source));
}


// ---------------------------------------------------------------------------------------------------------------------
// Matching algorithms
// ---------------------------------------------------------------------------------------------------------------------
// A match returns a view of matched characters when a scan is successful

// --------------------------------------------------
// Match beginning
// --------------------------------------------------
template <typename T>
constexpr std::optional<string_view> match_with (string_view& source, T t)
{
    string_view copy = source;
    int length = len(t);

    if (!scan_with(source, move(t)))    return {};

    return copy.substr(0, length);
}


template <typename... Args>
constexpr std::optional<string_view> match_with (string_view& source, basic_scanner<Args...> s, Args&&... args)
{
    string_view copy = source;
    if (!s(source, forward<Args>(args)...))    return {};

    return skipped(copy, source);
}


// --------------------------------------------------
// Extended matching
// --------------------------------------------------
template <typename T>
constexpr std::optional<string_view> match_while (string_view& source, T&& t)
{
    return match_with(source, scan_while, forward<T>(t));
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view> match_while (string_view& source, Pred&& p, Args&&... args)
{
    return match_with(source, scan_while, forward<Pred>(p), forward<Args>(args)...);
}


template <typename T>
constexpr std::optional<string_view> match_while_not (string_view& source, T&& t)
{
    return match_with(source, scan_while_not, forward<T>(t));
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view> match_while_not (string_view& source, Pred&& p, Args&&... args)
{
    return match_with(source, scan_while_not, forward<Pred>(p), forward<Args>(args)...);
}


template <typename T>
constexpr std::optional<string_view> match_until (string_view& source, T&& t)
{
    return match_with(source, scan_until, forward<T>(t));
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view> match_until (string_view& source, Pred&& p, Args&&... args)
{
    return match_with(source, scan_until, forward<Pred>(p), forward<Args>(args)...);
}


// --------------------------------------------------
// Match combiners
// --------------------------------------------------
template <typename... Scanners>
constexpr std::optional<string_view> match_join (string_view& source, Scanners&&... s)
{
    return match_with(source, scan_join, forward<Scanners>(s)...);
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view>
match_scan_ws_before (string_view& source,
                      Pred&& p, Args&&... args,
                      char_predicate is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    if (!(scan_while(source, is_whitespace)))    return {};
    return match_with(source, forward<Pred>(p), forward<Args>(args)...);
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view>
match_scan_delim_before (string_view& source,
                         Pred&& p, Args&&... args,
                         char_predicate is_delimiter = [] (char c) constexpr -> bool { return c == ';'; },
                         char_predicate is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    scan_while(source, is_whitespace);
    if (!( scan_with(source, is_delimiter) && scan_while(source, is_whitespace) ))
        return {};

    return match_with(source, forward<Pred>(p), forward<Args>(args)...);
}


// Only return a tuple if all generator arguments are executed successfully
template <typename Result, typename Pred>
constexpr tuple<Result> generate_tuple_while (Pred p)
{
    return p().value_or(tuple<>{});
}


template <typename Result, typename Pred, typename... Preds>
constexpr tuple<Result> generate_tuple_while (Pred first, Preds... rest)
{
    auto a = first();
    if (!a)    return tuple<>{};

    return std::tuple_cat(*a, generate_tuple_while(rest...));
}


template <typename Result, typename... Args, typename... Scanners>
constexpr std::optional<Result>
match_sequence (string_view& source,
                basic_scanner<Args...> first, Scanners&&... rest,
                char_predicate is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    auto result = generate_tuple_while(first(source),
                                       match_scan_ws_before(source, forward<Scanners>(rest), is_whitespace)...);

    if (std::tuple_size<decltype(result)>(result) != sizeof...(rest) + 1)    return {};
    return result;
}


template <typename... Scanners, typename... Args, typename Result>
constexpr std::optional<Result>
match_delimiter (string_view& source,
                 basic_scanner<Args...> first, Scanners&&... rest,
                 char_predicate is_delimiter = [] (char c) constexpr -> bool { return c == ';'; },
                 char_predicate is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    auto result = generate_tuple_while(first(source),
                                       match_scan_delim_before(source, forward<Scanners>(rest), is_delimiter, is_whitespace)...);

    if (std::tuple_size<decltype(result)>(result) != sizeof... (rest) + 1)    return {};
    return result;
}


// ---------------------------------------------------------------------------------------------------------------------
// Scanning
// ---------------------------------------------------------------------------------------------------------------------

// Tentative architecture:
// Scanner:    identifies a lexical sequence, advancing a position
// Matcher:    structures a lexical sequence, returns a parse tree
// Lexer:      assigns meaning to a lexical sequence, returns a token sequence
// Pattern:    identifies a semantic sequence, returns a set of tokens
// Parser:     structures a semantic sequence, returns an abstract syntax tree
// Translator: assigns meaning to a semantic sequence, returns a block of code in a target language

namespace Scan {
    
// Does the same as with(CharT), but is nice sugar when passing a string-like value
template <typename CharT>
scanner lit (CharT literal)
{
    return [l = move(literal)] (string_view& source) -> bool
    {
        return scan_with(source, move(l));
    };
}


template <typename T>
scanner with (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_with(source, move(t));
    };
}


template <typename Pred, typename... Args>
scanner with (string_view& source, Pred p, Args... args)
{
    return [p = movep(p), ... args = move(args)] (string_view& source) -> bool
    {
        return scan_with(source, move(p), move(args)...);
    };
}


template <typename Pred, typename... Args>
scanner optional (Pred p, Args... args)
{
    return [p = move(p), ... args = move(args)] (string_view& source) -> bool
    {
        return scan_optional(source, move(p), move(args)...);
    };
}


template <typename Pred, typename... Args>
scanner scan_while (Pred p, Args... args)
{
    return [p = move(p), ... args = move(args)] (string_view& source) -> bool
    {
        return scan_while(source, move(p), move(args)...);
    };
}


template <typename T>
scanner stop_before (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_while_not(source, move(t));
    };
}


template <typename T>
scanner until (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_until(source, move(t));
    };
}


template <typename... Scanners>
scanner join (Scanners... scanners)
{
    return [... s = move(scanners)] (string_view& source) -> bool
    {
        return scan_join(source, move(s)...);
    };
}

} // namespace Scan


namespace Match {

// Does the same as with(CharT), but is nice sugar when passing a string-like value
template <typename CharT>
matcher<string_view> lit (CharT literal)
{
    return [l = move(literal)] (string_view& source) -> std::optional<string_view>
    {
        return match_with(source, move(l));
    };
}


template <typename T>
matcher<string_view> with (T t)
{
    return [t = move(t)] (string_view& source) -> std::optional<string_view>
    {
        return match_with(source, move(t));
    };
}


template <typename... Args>
matcher<string_view> with (string_view& source, basic_scanner<Args...> s, Args... args)
{
    return [s = move(s), ... args = move(args)] (string_view& source) -> std::optional<string_view>
    {
        return match_with(source, move(s), move(args)...);
    };
}


template <typename... Args>
matcher<string_view> match_while (basic_scanner<Args...> s, Args... args)
{
    return [s = move(s), ... args = move(args)] (string_view& source) -> std::optional<string_view>
    {
        return match_while(source, move(s), move(args)...);
    };
}


template <typename T>
matcher<string_view> stop_before (T t)
{
    return [t = move(t)] (string_view& source) -> std::optional<string_view>
    {
        return match_while_not(source, move(t));
    };
}


template <typename T>
matcher<string_view> until (T t)
{
    return [t = move(t)] (string_view& source) -> std::optional<string_view>
    {
        return match_until(source, move(t));
    };
}


template <typename... Scanners>
matcher<string_view> join (Scanners... scanners)
{
    return [... s = move(scanners)] (string_view& source) -> std::optional<string_view>
    {
        return match_join(source, move(s)...);
    };
}


// Default seq function treats spaces and tab as whitespace
struct sequencer
{
    char_predicate is_whitespace = [] (char c) -> bool { return c == ' ' || c == '\r'; };

    template <typename Result, typename... Scanners>
    matcher<Result> operator() (Scanners&&... s)
    {
        return operator()(forward<Scanners>(s)..., is_whitespace);
    }

    template <typename Result, typename... Scanners>
    matcher<Result> operator() (Scanners... s, char_predicate is_whitespace)
    {
        return [... s = move(s), p = move(is_whitespace)] (string_view& source) -> std::optional<Result>
        {
            return match_sequence(source, move(s)..., move(p));
        };
    }
} seq;


// Default delim function adds <optional whitespace, comma, whitespace> as a delimiter
struct delimited_sequencer
{
    char_predicate is_whitespace = [] (char c) -> bool { return c == ' ' || c == '\r'; };
    char_predicate is_delimiter  = [] (char c) -> bool { return c == ';'; };

    template <typename Result, typename... Scanners>
    matcher<Result> operator() (Scanners&&... s)
    {
        return operator() (forward<Scanners>(s)..., is_delimiter, is_whitespace);
    }
    
    template <typename Result, typename... Scanners>
    matcher<Result> operator() (Scanners&&... s, char_predicate&& is_delimiter)
    {
        return operator() (forward<Scanners>(s)..., forward<char_predicate>(is_delimiter), is_whitespace);
    }

    template <typename Result, typename... Scanners>
    matcher<Result> operator() (Scanners... s, char_predicate is_delimiter, char_predicate is_whitespace)
    {
        return [... s = move(s), p1 = move(is_delimiter), p2 = move(is_whitespace)]
               (string_view& source) -> std::optional<Result>
        {
            return match_delimited_sequence(source, move(s)..., move(p1), move(p2));
        };
    }
} delim;


} // namespace Match







//////


// Lexer until (Lexer lexer) {
//     return [&] (string_view source) -> std::optional<ParseTree> {
//         for (int i = 0;   i < len(source);   ++i) {
//             auto match = lexer(source.substr(i, source.end()));
//             if (match)   return std::make_tuple(source.substr(0, i), *match);
//         }
        
//         return {};
//     };
// }


// template <Lexer... L>
// Lexer first_of (L... lexers) {
//     return [&] (string_view source) -> std::optional<ParseTree> {
//         for (auto lexer : lexers) {
//             auto match = lexer(source);
//             if (match)    return std::make_tuple(*match);
//         }

//         return {};
//     };
// }


// template <Lexer... L>
// Lexer seq (L... lexers) {
//     return [&] (string_view source) -> std::optional<ParseTree> {
//         // First
//         auto match = lexers.front()(source);
//         if (!match)    return {};

//         auto matches = std::make_tuple(*match);
//         source.remove_prefix(match->length());
        

//         // Rest
//         for (auto lexer : lexers | drop_view(1)) {
//             auto match = lexer(source);
//             if (!match)    return {};

//             auto matches = std::tuple_cat(matches, std::make_tuple(*match));
//             source.remove_prefix(match->length());
//         }

//         return matches;
//     };
// }



// // forever
// template <typename S, typename P, typename D>
// std::optional<std::vector<string_view>>
// match_amount (S&& source, P&& pattern, D&& delimiter = "") {
//     return match_amount(forward<S>(source), forward<P>(pattern), 0, -1, forward<D>(delimiter));
// }


// // exactly n
// template <typename S, typename P, typename D>
// std::optional<std::vector<string_view>>
// match_amount (S&& source, P&& pattern, int n, D&& delimiter = "") {
//     return match_amount(forward<S>(source), forward<P>(pattern), n, n, forward<D>(delimiter));
// }


// // at least min, at most max
// std::optional<std::vector<string_view>>
// match_amount (string_view source, string_view pattern, int min, int max, string_view delimiter = "") {
//     // fast checks
//     if (min <= 0)    return {};

//     auto match = match_literal(source, pattern);
//     if (!match)    return {};
//     std::vector<string_view> matches = {match.value()};


//     // check rest
//     if (max == -1)    max = string_view::npos;

//     std::string new_pattern_string;

//     if (delimiter != "") {
//         new_pattern_string = delimiter.data();
//         new_pattern_string += pattern.data();
//         pattern = new_pattern_string;
//     }

//     for (int i = 1;    i < max;    ++i) {
//         match = match_literal(source, pattern);

//         if (!match) {
//             if (i < min)    return {};
//             break;
//         }

//         matches.push_back(match.value());
//     }
    

//     return matches;
// }


// // ---------------------------------------------------------------------------------------------------------------------
// // Syntactic sugar
// // ---------------------------------------------------------------------------------------------------------------------
// Pattern operator| (Pattern left, const Pattern& right) {
//     return lexer_any(left, right);
// }


// // ---------------------------------------------------------------------------------------------------------------------
// // Pattern sugar
// // ---------------------------------------------------------------------------------------------------------------------
// Lexer at_least   (int n, Pattern pattern)         { return rep(pattern, n, -1); }
// Lexer at_most    (int n, Pattern pattern)         { return rep(pattern, 0, n); }
// Lexer n_times    (int n, Pattern pattern)         { return rep(pattern, n); }
// Lexer optional   (Pattern pattern)                { return rep(pattern, 0, 1); }
// Lexer from_to    (Pattern from, Pattern to)       { return seq {from, until(to), to}; }
// Lexer from_until (Pattern from, Pattern until)    { return seq {from, until(until)}; }


// // ---------------------------------------------------------------------------------------------------------------------
// // Lexical patterns
// // ---------------------------------------------------------------------------------------------------------------------

// Pattern digit    = any {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
// Pattern digits   = at_least(1, digit);
// Pattern integer  = digits;
// Pattern decimal  = join {digits, ".", digits};
// Pattern lower    = any {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
//                         "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
// Pattern upper    = any {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
//                         "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
// Pattern letter   = any {lower, upper};
// Pattern alphanum = any {letter, digit};
// Pattern newline  = any {"\r\n", "\n", "\r"};

// Lexer string_double (Pattern escape = lit("\\"))    { return from_to(lit('"'), lit('"')); }
// Lexer string_single (Pattern escape = lit("\\"))    { return from_to(lit("'"), lit("'")); }
// Lexer string        (Pattern escape = lit("\\"))    { return any {string_double(escape), string_single(escape)}; }
// Lexer line_comment  (Pattern start)                 { return from_to(start, newline); }


// // Pattern decimal = ("0" | "1" | "2")[1]++ ~ "." ~ ("0" | "1" | "2")[1]++
// Pattern decimal = join(rep(any(lit("0"), lit("1"), lit("2")), 1, -1),
//                        lit("."),
//                        rep(any(lit("0"), lit("1"), lit("2")), 1, -1));
// ParseTree decimal_match = decimal("101.2201");
// // decltype(decimal_match) == tuple<tuple<sv, sv, sv>, sv, tuple<sv, sv, sv, sv>>
// // ParseTree decimal_match = std::make_tuple(std::make_tuple("1", "0", "1"),
// //                                           ".",
// //                                           std::make_tuple("2", "2", "0", "1"))
// // 101.2201
// auto [integer, dot, fraction] = decimal_match;
// std::string decimal_string = integer + dot + fraction;
// double      decimal_value  = std::stod(decimal_string);



// ---------------------------------------------------------------------------------------------------------------------
// Grammatical patterns
// ---------------------------------------------------------------------------------------------------------------------
// indent
// block_comment(open, close)
// group(open, middle, close)
// list
// block
// binary operation
// function application







#endif