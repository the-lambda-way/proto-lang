#ifndef PATTERN_H
#define PATTERN_H

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


// ---------------------------------------------------------------------------------------------------------------------
// Primitive types
// ---------------------------------------------------------------------------------------------------------------------

// start: first index of a positive match, undefined for a negative match
// end:   index after last character of a positive match, undefined for a negative match
class Pattern {
public:
    virtual bool operator() (std::string source, int pos)   { return false; }

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
    any (char a[], char b[]);    // override vector constructor of 2 char[] params
    any (std::vector<Pattern>     patterns);
    any (std::vector<std::string> lit_patterns);

    bool operator() (std::string source, int pos) override;

    int which;

private:
    std::vector<Pattern> patterns;
};


class seq : public Pattern {
public:
    seq (char a[], char b[]);    // override vector constructor of 2 char[] params
    seq (std::vector<Pattern> patterns);
    seq (std::vector<Pattern> patterns, Pattern separator);
    seq (std::vector<std::string> lit_patterns);
    seq (std::vector<std::string> lit_patterns, Pattern separator);

    bool operator() (std::string source, int pos) override;

    std::vector<Pattern> patterns;
};


class until : public Pattern {
public:
    until (Pattern pattern);

    bool operator() (std::string source, int pos) override;
    
    Pattern pattern;
};


// max:     pass -1 to match as many as possible
// matches: stores a copy of each match of *pattern*
// amount:  size of *matches*
class rep : public Pattern {
public:
    rep (Pattern pattern);                      // forever
    rep (Pattern pattern, int n);               // exactly n
    rep (Pattern pattern, int min, int max);    // at least min, at most max

    bool operator() (std::string source, int pos) override;

    std::vector<Pattern> matches;
    int                  amount;

private:
    Pattern pattern;
    int min;
    int max;
};


}    // namespace PatLib


namespace PatDef {
namespace PL = PatLib;

// ---------------------------------------------------------------------------------------------------------------------
// Pattern sugar
// ---------------------------------------------------------------------------------------------------------------------

PL::Pattern at_least   (int n, PL::Pattern pattern)             { return PL::rep(pattern, n, -1); }
PL::Pattern at_most    (int n, PL::Pattern pattern)             { return PL::rep(pattern, 0, n); }
PL::Pattern n_times    (int n, PL::Pattern pattern)             { return PL::rep(pattern, n); }
PL::Pattern optional   (PL::Pattern pattern)                    { return PL::rep(pattern, 0, 1); }
PL::Pattern from_to    (PL::Pattern from, PL::Pattern to)       { return PL::seq({from, PL::until(to), to}); }
PL::Pattern from_until (PL::Pattern from, PL::Pattern until)    { return PL::seq({from, PL::until(until)}); }


// ---------------------------------------------------------------------------------------------------------------------
// Lexical patterns
// ---------------------------------------------------------------------------------------------------------------------

PL::Pattern digit    = PL::any({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
PL::Pattern digits   = at_least(1, digit);
PL::Pattern integer  = digits;
PL::Pattern decimal  = PL::seq({digits, PL::lit("."), digits});
PL::Pattern lower    = PL::any({"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                                "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"});
PL::Pattern upper    = PL::any({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                                "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"});
PL::Pattern letter   = PL::any({lower, upper});
PL::Pattern alphanum = PL::any({letter, digit});
PL::Pattern newline  = PL::any({"\r\n", "\n", "\r"});

PL::Pattern string_double (PL::Pattern escape = PL::lit("\\"))    { return from_to(PL::lit('"'), PL::lit('"')); }
PL::Pattern string_single (PL::Pattern escape = PL::lit("\\"))    { return from_to(PL::lit("'"), PL::lit("'")); }
PL::Pattern string        (PL::Pattern escape = PL::lit("\\"))    { return PL::any({string_double(escape), string_single(escape)}); }
PL::Pattern line_comment  (PL::Pattern start)                     { return from_to(start, newline); }


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