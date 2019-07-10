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
*/


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
    lit(char value);
    lit(std::string value);

    bool operator() (std::string source, int pos) override;

    std::string value;
    int         size;
};


class until : public Pattern {
public:
    until(Pattern pattern);

    bool operator() (std::string source, int pos) override;
    
    Pattern pattern;
};


// which: first match in *patterns*
class any : public Pattern {
public:
    any(std::vector<Pattern>     patterns);
    any(std::vector<std::string> lit_patterns);

    bool operator() (std::string source, int pos) override;

    int which;

private:
    std::vector<Pattern> patterns;
};


class seq : public Pattern {
public:
    seq(std::vector<Pattern> patterns);
    seq(std::vector<Pattern> patterns, Pattern separator);
    seq(std::vector<std::string> lit_patterns);
    seq(std::vector<std::string> lit_patterns, Pattern separator);

    bool operator() (std::string source, int pos) override;

    std::vector<Pattern> patterns;
};


// max:     pass -1 to match as many as possible
// matches: stores a copy of each match of *pattern*
// amount:  size of *matches*
class rep : public Pattern {
public:
    rep(Pattern pattern);                      // forever
    rep(Pattern pattern, int n);               // exactly n
    rep(Pattern pattern, int min, int max);    // at least min, at most max

    bool operator() (std::string source, int pos) override;

    std::vector<Pattern> matches;
    int                  amount;

private:
    Pattern pattern;
    int min;
    int max;
};


// ---------------------------------------------------------------------------------------------------------------------
// Pattern generators
// ---------------------------------------------------------------------------------------------------------------------

Pattern at_least   (Pattern pattern, int n)         { return rep(pattern, n, -1); }
Pattern at_most    (Pattern pattern, int n)         { return rep(pattern, 0, n); }
Pattern n_times    (Pattern pattern, int n)         { return rep(pattern, n); }
Pattern optional   (Pattern pattern)                { return rep(pattern, 0, 1); }
Pattern from_to    (Pattern from, Pattern to)       { return seq({from, until(to), to}); }
Pattern from_until (Pattern from, Pattern until)    { return seq({from, ::until(until)}); }


// ---------------------------------------------------------------------------------------------------------------------
// Lexical patterns
// ---------------------------------------------------------------------------------------------------------------------

Pattern digit    = any({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
Pattern digits   = rep(digit);
Pattern integer  = digits;
Pattern decimal  = seq({digits, lit("."), digits});
Pattern lower    = any({"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"});
Pattern upper    = any({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"});
Pattern letter   = any({lower, upper});
Pattern alphanum = any({letter, digit});
Pattern newline  = any({"\r\n", "\n", "\r"});

Pattern string_double (Pattern escape = lit("/"))    { return from_to(lit('"'), lit('"')); }
Pattern string_single (Pattern escape = lit("/"))    { return from_to(lit("'"), lit("'")); }
Pattern string        (Pattern escape = lit("/"))    { return any({string_double(escape), string_single(escape)}); }
Pattern line_comment  (Pattern start)                { return from_to(start, newline); }


// ---------------------------------------------------------------------------------------------------------------------
// Grammatical patterns
// ---------------------------------------------------------------------------------------------------------------------



#endif