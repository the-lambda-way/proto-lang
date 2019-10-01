#ifndef PATTERN_H
#define PATTERN_H

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <memory>
#include <string>
#include <vector>

/*
************************************************************************************************************************
* Pattern
* The Pattern library provides functions that recognize a pattern beginning at a position of a string, keeping track of
*     source locations, while avoiding any string processing.
* 
************************************************************************************************************************

Source locations are saved as *start* (inclusive) and *end* (exclusive) indices for all pattern matchers.

Here are the primitive matching operations:
    * lit
    * any
    * seq
    * until
    * rep (min, max)

There are several derived matching operations:
    * at_least   = rep(p, n, -1)
    * at_most    = rep(p, 0, n)
    * n_times    = rep(p, n, n)
    * optional   = rep(p, 0, 1)
    * from_to    = seq(p, until(p2), p2)
    * from_until = seq(p, until(p2))
    
There are several built-in lexical patterns:
    * digit
    * digits
    * integer
    * decimal
    * lower
    * upper
    * letter
    * alphanum
    * newline
    * string_double(escape = "/")
    * string_single(escape = "/")
    * string(escape = "/")
    * line_comment(start)
    
As well as some more advanced grammatical patterns:
    * indent
    * block_comment(open, close)
    * group(open, middle, close)
    * list
    * block
    * binary operation
    * function application

TODO: Consider distinguishing compound patterns from simple patterns. Compound patterns will treat each constituent
    element as distinct. Simple patterns will treat them as parts of a whole. Simple patterns would then be able to
    remove some bookkeeping code.

*/

namespace PatLib {
using pPattern = std::unique_ptr<Pattern>;

// ---------------------------------------------------------------------------------------------------------------------
// Primitive types
// ---------------------------------------------------------------------------------------------------------------------
// start: first index of a positive match, undefined for a negative match
// end:   index after last character of a positive match, undefined for a negative match
class Pattern {
public:
    virtual bool operator() (std::string source, int pos) = 0;

    int start = 0;
    int end   = 0;
};


// value: base string value
// size:  size of *value*
class lit : public Pattern {
public:
    lit (char value);
    lit (std::string value);

    bool operator() (std::string source, int pos) override;

    std::string value;
    int         size;
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


}    // namespace PatLib


namespace PatDef {
namespace PL = PatLib;

// ---------------------------------------------------------------------------------------------------------------------
// Pattern sugar
// ---------------------------------------------------------------------------------------------------------------------

PL::pPattern at_least   (int n, PL::pPattern pattern)              { return PL::rep(std::move(pattern), n, -1); }
PL::pPattern at_most    (int n, PL::pPattern pattern)              { return PL::rep(std::move(pattern), 0, n); }
PL::pPattern n_times    (int n, PL::pPattern pattern)              { return PL::rep(std::move(pattern), n); }
PL::pPattern optional   (PL::pPattern pattern)                     { return PL::rep(std::move(pattern), 0, 1); }
PL::pPattern from_to    (PL::pPattern from, PL::pPattern to)       { return PL::seq({std::move(from), PL::until(to), std::move(to)}); }
PL::pPattern from_until (PL::pPattern from, PL::pPattern until)    { return PL::seq({std::move(from), PL::until(until)}); }


// ---------------------------------------------------------------------------------------------------------------------
// Lexical patterns
// ---------------------------------------------------------------------------------------------------------------------

PL::pPattern digit    = PL::any({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
PL::pPattern digits   = at_least(1, digit);
PL::pPattern integer  = digits;
PL::pPattern decimal  = PL::seq({digits, PL::lit("."), digits});
PL::pPattern lower    = PL::any({"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                                "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"});
PL::pPattern upper    = PL::any({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                                "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"});
PL::pPattern letter   = PL::any({lower, upper});
PL::pPattern alphanum = PL::any({letter, digit});
PL::pPattern newline  = PL::any({"\r\n", "\n", "\r"});

PL::pPattern string_double (PL::Pattern escape = PL::lit("\\"))    { return from_to(PL::lit('"'), PL::lit('"')); }
PL::pPattern string_single (PL::Pattern escape = PL::lit("\\"))    { return from_to(PL::lit("'"), PL::lit("'")); }
PL::pPattern string        (PL::Pattern escape = PL::lit("\\"))    { return PL::any({string_double(escape), string_single(escape)}); }
PL::pPattern line_comment  (PL::Pattern start)                     { return from_to(start, newline); }


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


}    // namespace PatDef


#endif