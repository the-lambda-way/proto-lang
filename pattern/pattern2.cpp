#ifndef PATTERN_H
#define PATTERN_H

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
#include <utility>        // std::pair

#include <range/v3/all.hpp>
using namespace ranges;
using std::string_view;


/***********************************************************************************************************************
* Pattern
* The Pattern library provides functions that recognize a pattern beginning at a position of a string.
* 
***********************************************************************************************************************/

// ---------------------------------------------------------------------------------------------------------------------
// Data types
// ---------------------------------------------------------------------------------------------------------------------
// Code -> ParseTree -> SyntaxTree

// Design:
// The library tries to be representation-agnostic, providing generic tools and concepts that model common abstractions
//     in the domain. The following terms are used througout the library.

// Patterns structure source
// Pattern         : a convenient representation (data format) of a parse tree, similar to EBNF form
// Pattern literal : matches a literal code element
// Pattern type    : matches a class of code elements
// Pattern concept : matches a combination of pattern types and pattern literals

// A Pattern is just one convenient way to represent a set of parsing actions. The library can be used without it.

// Datum           : a concrete instance of a Pattern
// Parser          : transforms a string into structured source code
// ParserObject    : a managed parser that maintains the results of a parse


// A Datum is a structured string.
class Datum {
public:
    const std::string head = "";
    const std::vector<Datum*> tail = { };
    Datum operator[] (size_t index) const    { return *tail[index]; }
};


// ---------------------------------------------------------------------------------------------------------------------
// Concepts
// ---------------------------------------------------------------------------------------------------------------------
// A Lexer transforms a sequence of input characters into a ParseTree
template <typename L, typename S, typename ParseTree>
concept bool Lexer =
    requires (L lexer, const S& source) {
        { std::invoke(*lexer, source) } -> std::pair<iterator_t<S>, ParseTree>;
};


// A Parser transforms a sequence of input characters into a SyntaxTree
template <typename P, typename S, typename SyntaxTree>
concept bool Parser =
    requires (P parser, const S& source) {
        { std::invoke(*parser, source) } -> std::pair<iterator_t<S>, SyntaxTree>;
};


// Code is a container of objects which can be compared and iterated over
template <typename C>
concept bool Code =
    requires (C code) {
        { range::begin(code) };
        { range::end(code)   };
};


// ---------------------------------------------------------------------------------------------------------------------
// Algorithms
// ---------------------------------------------------------------------------------------------------------------------
// These algorithms advance the source view upon a match, so that iteration through the source is automatically handled.


std::optional<string_view> match_literal (string_view& source, string_view pattern) {
    // fast checks
    if (source.front() != pattern.front())     return {};    // usual result
    if (source.length() < pattern.length())    return {};    // save a check every loop

    // check the rest
    int i = 1;
    
    for ( ; i != pattern.length(); ++i)
        if (source[i] != pattern[i])    return {};

    string_view result = source.substr(0, pattern.length());
    source.remove_prefix(i-1);

    return result;
}


// First return value is the subview that was skipped, before the pattern
// Second return value is the matched pattern
std::optional<std::pair<string_view, string_view>>
match_find (string_view& source, string_view pattern) {

    // If the pattern is a single character, then every character must be checked
    if (pattern.length() == 1) {
        auto searcher = std::default_searcher(pattern.begin(), pattern.end());
        auto [first, last] = searcher(source.begin(), source.end());

        if (first == source.end())    return {};

        string_view first_part(source.begin(), std::distance(source.begin(), first));
        string_view second_part(first, std::distance(first, last));
        return std::make_pair(first_part, second_part);
    }

    // If the pattern is more than one character, Boyer-Moore search may be more efficient
    else {
        auto searcher = std::boyer_moore_searcher(pattern.begin(), pattern.end());
        auto [first, last] = searcher(source.begin(), source.end());

        if (first == source.end())    return {};   

        string_view first_part(source.begin(), std::distance(source.begin(), first));
        string_view second_part(first, std::distance(first, last));
        return std::make_pair(first_part, second_part);
    }
}


template <range S, range P>
std::pair<iterator_t<S>, iterator_t<P>>
match_any (const S& source, const P& patterns) {
    for (const range& p : patterns) {
        auto [s_end, p_end] = mismatch(source, p);
        if (p_end == end(p))    return std::make_pair(s_end, &p);
    }

    return std::make_pair(begin(source), end(patterns));
}


// Change to return a copy of each subpattern match
template <typename End, range S, range P>
std::pair<End, std::vector<*S>> match_seq (const S& source, const P& patterns) {
    auto copy = subrange(source);

    for (const range& p : patterns) {
        auto [s_end, p_end] = mismatch(copy, p);
        if (p_end != end(p))    return subrange(source);
        copy.advance(iter_distance(copy.begin(), s_end));
    }

    return copy;
}


subrange match_amount (range source,
                       range pattern,
                       const int min = 0,
                       const int max = std::numeric_limits<int>::max()) {
    
    if constexpr (min < 0)    return subrange(source);

}



// ---------------------------------------------------------------------------------------------------------------------
// Primitive nodes
// ---------------------------------------------------------------------------------------------------------------------

template <range S, typename CharT = std::string>
class lit {
public:
    CharT value;

    iterator_t<S> operator() (const S&& source) {
        return match_begin(std::forward<S>(source), value);
    }
};







// which: first match in *patterns*
class any : public Pattern {
public:
    any (std::vector<pPattern>    patterns);
    any (std::vector<std::string> lit_patterns);
    any (char a[], char b[]);    // override vector constructor of 2 char[] params

    bool operator() (std::string source, int pos) override;

    int which;

private:
    std::vector<pPattern> patterns;
};


class seq : public Pattern {
public:
    seq (std::vector<pPattern> patterns);
    seq (std::vector<pPattern> patterns, pPattern separator);
    seq (std::vector<std::string> lit_patterns);
    seq (std::vector<std::string> lit_patterns, pPattern separator);
    seq (char a[], char b[]);    // override vector constructor of 2 char[] params

    bool operator() (std::string source, int pos) override;

    std::vector<pPattern> patterns;
};


class until : public Pattern {
public:
    until (pPattern pattern);

    bool operator() (std::string source, int pos) override;
    
    pPattern pattern;
};


// max:     pass -1 to match as many as possible
// matches: stores a copy of each match of *pPattern
// amount:  size of *matches*
class rep : public Pattern {
public:
    rep (pPattern pattern);                      // forever
    rep (pPattern pattern, int n);               // exactly n
    rep (pPattern pattern, int min, int max);    // at least min, at most max

    bool operator() (std::string source, int pos) override;

    std::vector<pPattern> matches;
    int                   amount;

private:
    pPattern pattern;
    int min;
    int max;
};



// ---------------------------------------------------------------------------------------------------------------------
// Syntactic sugar
// ---------------------------------------------------------------------------------------------------------------------
class Pattern {};


Pattern operator| (Pattern left, const Pattern& right) {
    return any(left, right);
}



// ---------------------------------------------------------------------------------------------------------------------
// Pattern sugar
// ---------------------------------------------------------------------------------------------------------------------

Pattern at_least   (int n, Pattern pattern)         { return rep(pattern, n, -1); }
Pattern at_most    (int n, Pattern pattern)         { return rep(pattern, 0, n); }
Pattern n_times    (int n, Pattern pattern)         { return rep(pattern, n); }
Pattern optional   (Pattern pattern)                { return rep(pattern, 0, 1); }
Pattern from_to    (Pattern from, Pattern to)       { return seq {from, until(to), to}; }
Pattern from_until (Pattern from, Pattern until)    { return seq {from, until(until)}; }


// ---------------------------------------------------------------------------------------------------------------------
// Lexical patterns
// ---------------------------------------------------------------------------------------------------------------------

Pattern digit    = any {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
Pattern digits   = at_least(1, digit);
Pattern integer  = digits;
Pattern decimal  = seq {digits, ".", digits};
Pattern lower    = any {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
Pattern upper    = any {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
Pattern letter   = any {lower, upper};
Pattern alphanum = any {letter, digit};
Pattern newline  = any {"\r\n", "\n", "\r"};

Pattern string_double (Pattern escape = lit("\\"))    { return from_to(lit('"'), lit('"')); }
Pattern string_single (Pattern escape = lit("\\"))    { return from_to(lit("'"), lit("'")); }
Pattern string        (Pattern escape = lit("\\"))    { return any {string_double(escape), string_single(escape)}; }
Pattern line_comment  (Pattern start)                 { return from_to(start, newline); }


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