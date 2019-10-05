#ifndef PATTERN_H
#define PATTERN_H

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <algorithm>
#include <functional>     // std::search searchers
#include <iostream>       // cout
#include <string>
#include <string_view>
#include <tuple>          // ParseTree
#include <type_traits>    // is_same concept
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

// Tentative architecture:
// Scanner:    identifies a lexical sequence, advancing a position only if a match is found
// Matcher:    structures a lexical sequence, returns a parse tree
// Lexer:      assigns meaning to a lexical sequence, returns a token
// Pattern:    identifies a semantic sequence, returns a sequence of tokens
// Parser:     structures a semantic sequence, returns an abstract syntax tree
// Translator: assigns meaning to a semantic sequence, returns a block of code in a target language


/***********************************************************************************************************************
* Pattern
* The Pattern library provides functions that recognize a pattern beginning at a position of a string.
*
***********************************************************************************************************************/

namespace PatLib {

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
//  Concepts
// ---------------------------------------------------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/concepts/same_as
// Will be available in C++20
namespace detail {
    template< class T, class U >
    concept bool SameHelper = std::is_same_v<T, U>;
}

template <class T, class U>
concept bool same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

// using char_predicate = bool (*) (char);

template <typename Pred>
concept bool char_predicate =
    // Callable<Pred> &&
    requires (Pred p, char c)
    {
        { p(c) } -> bool;
    };


// template <typename... Args>
// using basic_scanner = bool (*) (string_view, Args...);

// using scanner = basic_scanner<>;

template <typename Pred, typename... Args>
concept bool scanner =
    // Callable<Pred> &&
    requires (Pred p, string_view source, Args... args)
    {
        { p(source, args...) } -> bool;
    };

// template <typename Result, typename... Args>
// using basic_matcher = std::optional<Result> (*) (string_view, Args...);

// template <typename Result>
// using matcher = basic_matcher<Result>;

template <typename Pred>
concept bool matcher =
    // Callable<Pred> &&
    requires (Pred p, string_view source)
    {
        { p(source) } -> string_view;
    };


// ---------------------------------------------------------------------------------------------------------------------
//  Syntactic sugar
// ---------------------------------------------------------------------------------------------------------------------
constexpr int len (const char& c)             { return 1; }
constexpr int len (const char_predicate& p)   { return 1; }
constexpr int len (const string_view& s)      { return s.length(); }

// Bounds checking is appropriate, because it needs to be done anyway as part of any scanner.
constexpr char peek (string_view& source)                       { return source.empty() ? '\0' : source.front(); }
constexpr char peek2 (string_view& source)                      { return len(source) <= 1 ? '\0' : source[1]; }
constexpr void advance (string_view& source, int amount = 1)    { source.remove_prefix(amount); }

// Warning: this is more expensive than making an extra copy of source
constexpr void backtrack (string_view& source, int amount = 1)
{
    auto char_ptr = &*(source.begin() - amount);
    source = string_view(char_ptr, std::distance(char_ptr, source.end()));
}

constexpr string_view skipped (string_view source, string_view copy)
{
    return source.substr(0, std::distance(source.begin(), copy.begin()));
}

std::string to_string (string_view s)    { return {s.data(), s.length()}; }


// ---------------------------------------------------------------------------------------------------------------------
// Scanning algorithms
// ---------------------------------------------------------------------------------------------------------------------
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


template <typename T>
constexpr bool scan_when (string_view& source, T t)
{
    if (!starts_with(source, t))    return false;
    advance(source, len(t));
    return true;
}


template <typename T>
constexpr bool scan_optional (string_view& source, T&& t)
{
    scan_when(source, forward<T>(t));
    return true;
}


template <typename... Args>
constexpr bool scan_optional (string_view& source, const scanner& s, Args... args)
{
    s(source, forward<Args>(args)...);
    return true;
}


template <typename T>
constexpr bool scan_while (string_view& source, T t)
{
    if (!scan_when(source, t))    return false;
    while (scan_when(source, t));
    return true;
}


template <typename T>
constexpr bool scan_while_not (string_view& source, T t)
{
    while (!starts_with(source, t) && !source.empty())    advance(source);
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
constexpr bool scan_until (string_view& source, T t)
{
    if (!scan_while_not(source, move(t)))    return false;
    advance(source, len(t));
    return true;
}


template <typename... Scanners>
constexpr bool scan_join (string_view& source, const Scanners&... s)
{
    string_view copy = source;

    if (!(... && s(copy)))    return false;

    source = copy;
    return true;
}


template <typename... Scanners>
constexpr bool scan_any (string_view& source, const Scanners&... s)
{
    return (... || s(source));
}

template <typename Arg>
auto make_simple_scanner (scanner s, Arg arg)
{
    return [s = move(s), arg = move(arg)] (string_view& source) -> bool
    {
        return s(source, arg);
    };
}


template <typename... Args>
auto make_simple_scanner (scanner s, Args... args)
{
    return [s = move(s), ...args = move(args)] (string_view& source) -> bool
    {
        return s(source, args...);
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Scanning
// ---------------------------------------------------------------------------------------------------------------------
namespace Scan {

// // Does the same as with(literal), but is nice sugar when passing a string-like value
// template <typename CharT>
// auto lit (CharT literal)
// { return make_simple_scanner(scan_when, move(literal)); }


// template <typename T>
// auto when (T t)
// { return make_simple_scanner(scan_when, move(t)); }


// template <typename T>
// auto optional (T t)
// { return make_simple_scanner(scan_optional, move(t)); }


// template <typename T>
// auto while_it (T t)
// { return make_simple_scanner(PatLib::scan_while, move(t)); }


// template <typename T>
// auto stop_before (T t)
// { return make_simple_scanner(scan_while_not, move(t)); }


// template <typename T>
// auto until (T t)
// { return make_simple_scanner(scan_until, move(t)); }


// template <typename... Scanners>
// auto join (const Scanners&... scanners)
// { return make_simple_scanner(scan_join, move(scanners)...); }


// template <typename... Scanners>
// auto any (const Scanners&... scanners)
// { return make_simple_scanner(scan_any, move(scanners)...); }

template <typename CharT>
auto lit (CharT literal)
{
    return [literal = move(literal)] (string_view& source) -> bool
    {
        return scan_when(source, literal);
    };
}


template <typename T>
auto when (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_when(source, t);
    };
}

template <typename T>
auto optional (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_optional(source, t);
    };
}

template <typename T>
auto while_it (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_while(source, t);
    };
}

template <typename T>
auto stop_before (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_while_not(source, t);
    };
}

template <typename T>
auto until (T t)
{
    return [t = move(t)] (string_view& source) -> bool
    {
        return scan_until(source, t);
    };
}

template <typename... Scanners>
auto join (const Scanners&... scanners)
{
    return [...scanners = move(scanners)] (string_view& source) -> bool
    {
        return scan_join(source, scanners...);
    };
}

template <typename... Scanners>
auto any (const Scanners&... scanners)
{
    return [...scanners = move(scanners)] (string_view& source) -> bool
    {
        return scan_any(source, scanners...);
    };
}
} // namespace Scan


// ---------------------------------------------------------------------------------------------------------------------
// Matching algorithms
// ---------------------------------------------------------------------------------------------------------------------
// A match returns a std::optional containing a view of matched characters when a scan is successful

template <typename T>
constexpr std::optional<string_view> match_when (string_view& source, T t)
{
    string_view copy = source.substr(0, len(t));

    if (!scan_when(source, move(t)))    return {};

    return copy;
}


template <typename... Args>
constexpr std::optional<string_view> match_when (string_view& source, const scanner& s, Args&&... args)
{
    string_view copy = source;

    if (!s(source, forward<Args>(args)...))    return {};

    return skipped(copy, source);
}


template <typename T>
constexpr std::optional<string_view> match_optional (string_view& source, T&& t)
{
    auto match = match_when(source, forward<T>(t));
    return match.value_or(string_view(""));
}


template <scanner S, typename... Args>
constexpr std::optional<string_view> match_optional (string_view& source, const S& s, Args&&... args)
{
    auto match = match_when(source, forward<S>(s), forward<Args>(args)...);
    return match.value_or(string_view(""));
}


template <typename T>
constexpr std::optional<string_view> match_while (string_view& source, T&& t)
{ return match_when(source, scan_while, forward<T>(t)); }


template <typename T>
constexpr std::optional<string_view> match_while_not (string_view& source, T&& t)
{ return match_when(source, scan_while_not, forward<T>(t)); }


template <typename T>
constexpr std::optional<string_view> match_until (string_view& source, T&& t)
{ return match_when(source, scan_until, forward<T>(t)); }


template <typename... Scanners>
constexpr std::optional<string_view> match_join (string_view& source, const Scanners&&... s)
{ return match_when(source, scan_join, forward<Scanners>(s)...); }


template <typename Pred, typename... Args>
constexpr std::optional<string_view>
match_scan_ws_before (string_view& source,
                      Pred&& p, Args&&... args,
                      const char_predicate& is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    if (!(scan_while(source, is_whitespace)))    return {};
    return match_when(source, forward<Pred>(p), forward<Args>(args)...);
}


template <typename Pred, typename... Args>
constexpr std::optional<string_view>
match_scan_delim_before (string_view& source,
                         Pred&& p, Args&&... args,
                         const char_predicate& is_delimiter = [] (char c) constexpr -> bool { return c == ';'; },
                         const char_predicate& is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    scan_while(source, is_whitespace);
    if (!( scan_when(source, is_delimiter) && scan_while(source, is_whitespace) ))
        return {};

    return match_when(source, forward<Pred>(p), forward<Args>(args)...);
}


// Only return a tuple if all generator arguments are executed successfully
template <typename Result, typename Pred>
constexpr std::optional<tuple<Result>> generate_tuple_while (Pred p)
{
    return p().value_or(tuple<>{});
}


template <typename Result, typename Pred, typename... Preds>
constexpr std::optional<tuple<Result>> generate_tuple_while (Pred first, Preds... rest)
{
    auto a = first();
    if (!a)    return tuple<>{};

    return std::tuple_cat(*a, generate_tuple_while(rest...));
}


template <typename Result, typename Scanner, typename... Scanners>
constexpr std::optional<tuple<Result>>
match_sequence (string_view& source,
                Scanner&& first, Scanners&&... rest,
                const char_predicate& is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    auto result = generate_tuple_while([&] () { return first(source); },
                                       [&] () { return match_scan_ws_before(source,
                                                                            forward<Scanners>(rest),
                                                                            is_whitespace); }...);

    if (std::tuple_size<decltype(result)>(result) != sizeof...(rest) + 1)    return {};
    return result;
}


template <typename Result, typename Scanner, typename... Scanners>
constexpr std::optional<Result>
match_delimiter (string_view& source,
                 Scanner&& first, Scanners&&... rest,
                 const char_predicate& is_delimiter = [] (char c) constexpr -> bool { return c == ';'; },
                 const char_predicate& is_whitespace = [] (char c) constexpr -> bool { return c == ' ' || c == '\r'; })
{
    auto result = generate_tuple_while([&] () { first(source); },
                                       [&] () { match_scan_delim_before(source,
                                                                        forward<Scanners>(rest),
                                                                        is_delimiter,
                                                                        is_whitespace); }...);

    if (std::tuple_size<decltype(result)>(result) != sizeof... (rest) + 1)    return {};
    return result;
}


template <typename... Scanners>
constexpr std::optional<string_view> match_any (string_view& source, const Scanners&... s)
{
    return match_when(source, scan_any, forward<Scanners>(s)...);
}


template <typename... Args>
auto make_simple_matcher (matcher m, Args... args)
{
    return [m = move(m), ...args = move(args)] (string_view& source) -> string_view
    {
        return m(source, args...);
    };
}


namespace Match {

// Does the same as with(literal), but is nice sugar when passing a string-like value
template <typename CharT>
auto lit (CharT literal)
{ return make_simple_matcher(match_when, move(literal)); }


template <typename T>
auto when (T t)
{ return make_simple_matcher(match_when, move(t)); }


template <typename T>
auto optional (T t)
{ return make_simple_matcher(match_optional, move(t)); }


template <typename T>
auto while_it (T t)
{ return make_simple_matcher(PatLib::match_while, move(t)); }


template <typename T>
auto stop_before (T t)
{ return make_simple_matcher(match_while_not, move(t)); }


template <typename T>
auto until (T t)
{ return make_simple_matcher(match_until, move(t)); }


template <typename... Scanners>
auto join (Scanners... scanners)
{ return make_simple_matcher(match_join, move(scanners)...); }


template <typename... Scanners>
auto any (const Scanners&... scanners)
{ return make_simple_matcher(match_any, move(scanners)...); }


// // Does the same as with(CharT), but is nice sugar when passing a string-like value
// template <typename CharT>
// auto lit (CharT literal)
// {
//     return [l = move(literal)] (string_view& source) -> string_view
//     {
//         return match_when(source, move(l));
//     };
// }


// template <typename... Args>
// auto when (Args... args)
// {
//     return [... args = move(args)] (string_view& source) -> string_view
//     {
//         return match_when(source, move(args)...);
//     };
// }


// template <typename T>
// auto match_while (T t)
// {
//     return [t = move(t)] (string_view& source) -> string_view
//     {
//         return PatLib::match_while(source, move(t));
//     };
// }


// template <typename T>
// auto stop_before (T t)
// {
//     return [t = move(t)] (string_view& source) -> string_view
//     {
//         return match_while_not(source, move(t));
//     };
// }


// template <typename T>
// auto until (T t)
// {
//     return [t = move(t)] (string_view& source) -> string_view
//     {
//         return match_until(source, move(t));
//     };
// }


// template <typename... Scanners>
// auto join (Scanners... scanners)
// {
//     return [... s = move(scanners)] (string_view& source) -> string_view
//     {
//         return match_join(source, move(s)...);
//     };
// }


// Default seq function treats spaces and tab as whitespace
template <char_predicate P>
struct sequencer
{
    P is_whitespace = [] (char c) -> bool { return c == ' ' || c == '\r'; };

    template <typename... Scanners>
    auto operator() (Scanners&&... s)
    {
        return operator()(forward<Scanners>(s)..., is_whitespace);
    }

    template <typename Result, typename... Scanners>
    auto operator() (Scanners... s, char_predicate is_whitespace)
    {
        return [... s = move(s), p = move(is_whitespace)] (string_view& source) -> std::optional<tuple<Result>>
        {
            return match_sequence(source, move(s)..., move(p));
        };
    }
};

sequencer<bool(*)(char)> seq;


// Default delim function adds <optional whitespace, comma, whitespace> as a delimiter
template <char_predicate P1, char_predicate P2>
struct delimited_sequencer
{
    P1 is_whitespace = [] (char c) -> bool { return c == ' ' || c == '\r'; };
    P2 is_delimiter  = [] (char c) -> bool { return c == ';'; };

    template <typename... Scanners>
    auto operator() (Scanners&&... s)
    {
        return operator() (forward<Scanners>(s)..., is_delimiter, is_whitespace);
    }

    template <typename... Scanners>
    auto operator() (Scanners&&... s, char_predicate is_delimiter)
    {
        return operator() (forward<Scanners>(s)..., move(is_delimiter), is_whitespace);
    }

    template <typename Result, typename... Scanners>
    auto operator() (Scanners... s, char_predicate is_delimiter, char_predicate is_whitespace)
    {
        return [... s = move(s), p1 = move(is_delimiter), p2 = move(is_whitespace)]
               (string_view& source) -> std::optional<Result>
        {
            return match_delimited_sequence(source, move(s)..., move(p1), move(p2));
        };
    }
};

delimited_sequencer<bool(*)(char), bool(*)(char)> delim;

} // namespace Match





// What to do next:
// When the library is good, model after PEGs, or OMeta
// Library is close enough already to transition





namespace Lex {

    template <typename Lexer, typename Matcher>
    auto apply (Lexer l, Matcher m)
    {
        return [l = move(l), m = move(m)] (string_view& source) -> bool
        {
            auto n = m(source);
            if (!n)    return false;

            l(*n);
            return true;
        };
    }

} // namespace Lexer




//////

// A lexer should take a parse_tree and return void


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





} // namespace PatLib

#endif
