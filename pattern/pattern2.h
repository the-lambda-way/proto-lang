#ifndef PATTERN_H
#define PATTERN_H

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <iostream>
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
// Data types
// ---------------------------------------------------------------------------------------------------------------------
// Desired properties:
// Lightweight: They will be used liberally throughout code.
// Efficient:   Parsing takes a significant portion of compilation time in large languages.
// Copyable:    Copy semantics are simpler to reason about and more flexible.
// Polymorphic: Will be used by many different constructs. A single point of entry is preferred.
// Recursive:   Nested patterns.

// These requirements are a good fit for algebraic data types over inheritance.
// Design guided by https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/


// ---------------------------------------------------------------------------------------------------------------------
// Primitive types
// ---------------------------------------------------------------------------------------------------------------------


// Implementation Guide: https://www.youtube.com/watch?reload=9&v=QGcVXgEVMJg


// value: base string value
// size:  size of *value*
class lit {
public:
    lit (const char value);
    lit (const std::string value);

    bool operator() (const std::string source, int pos);

    int start;
    int end;
    const std::string value;
    const int         size;
};

using Pattern = std::vector<lit>;

bool parse (const Pattern& pattern, const std::string source, int pos) {
    for (auto p : pattern) {
        if (!p(source, pos))    { return false; }
        else                    { pos = p.end; }
    }
    return true;
}


int main () {
    Pattern pattern;
    pattern.emplace_back(lit("Hello"));
    pattern.emplace_back(lit(", world!"));

    if (parse(pattern, "Hello, world!", 0)) {
        for (const auto p : pattern)
            std::cout << "(" << p.start << ", " << p.end << ")" << std::endl;
    }
    else {
        std::cout << "No match." << std::endl;
    }
}


}    // namespace PatLib




#endif