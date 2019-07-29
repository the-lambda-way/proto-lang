#ifndef PATTERN_H
#define PATTERN_H

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
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
    * until
    * any
    * seq
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
// Data types
// ---------------------------------------------------------------------------------------------------------------------
// Desired properties:
// Lightweight: They will be used liberally throughout code.
// Efficient:   Parsing takes a significant portion of compilation time in large languages.
// Copyable:    Copy semantics are simpler to reason about and more flexible.
// Polymorphic: Will be used by many different constructs. A single point of entry is preferred.
// Recursive:   Nested patterns.


// Design:
// Pattern literal: a literal description of source code
// Pattern type:    a class of pattern literals
// Pattern:         the type of a pattern type
// Datum:           a concrete instance of a Pattern
// Parser:          transforms a string into structured source code


// ---------------------------------------------------------------------------------------------------------------------
// Data Types
// ---------------------------------------------------------------------------------------------------------------------
using std::size_t;

class Datum {
public:
    Datum () { }
    Datum (std::string id, std::initializer_list<Datum> members) : members(members.begin(), members.end()) { };
    
    std::string id = "";
    std::vector<Datum*> members = {};

    Datum& operator[] (size_t index)    { return *members[index]; }
};


// A SourceView is integer pair demarking a span of source code, with no pointer to the original source.
// std::string_view throws std::out_of_range if if start > source.size() when calling view(source)
class SourceView {
public:
    // Validity
    constexpr static auto npos = size_t(-1);
    operator bool() const          { return start != npos; }
    SourceView (const bool& value) { }

    
    // Metadata
    SourceView (size_t start, size_t end) : start(std::move(start)), end(std::move(end)) { }
    const size_t start = npos;
    const size_t end   = npos;

    std::string_view view (const std::string& source) const {
        return std::string_view(source).substr(start, end - start);
    }

    std::string get (const std::string& source) const {
        return source.substr(start, end - start);
    }

    size_t size()    { return end - start; }
};


class DatumView : public SourceView {
public:
    // Structure
    std::vector<SourceView> members;

    SourceView operator[] (const size_t& index)    { return members[index]; }

    std::vector<std::string_view> view_structure (const std::string& source) const {
        std::vector<std::string_view> result;
        result.reserve(members.size());

        for (auto m : members)    result.emplace_back(std::move(m));

        return result;
    }

    Datum get_datum (const std::string& source) const {
        Datum result;
        
    }
};


// A Parser transforms a sequence of input characters into a SyntaxTree
template <typename P>
concept bool Parser =
    requires (P parser, const std::string& source, int start) {
    { operator()(const parser&, source, start) } -> SyntaxTree;
    { to_string(parser&)                       } -> std::string;
};


// ---------------------------------------------------------------------------------------------------------------------
// Literal Parsers
// ---------------------------------------------------------------------------------------------------------------------

SourceView parse (const std::string& lit_pattern, const std::string& source, const int& start) {
    // fast checks
    if (lit_pattern[0] != source[start])               return false;    // usual result
    if (source.size() - start < lit_pattern.size())    return false;    // faster than checking every loop

    // check the rest
    int end;
    
    for (end = start + 1;   end < start + lit_pattern.size();   ++end)
        if (lit_pattern[end - start] != source[end])    return false;

    return {start, end};
}


DatumView parse_datum (const Datum& lit_pattern, const std::string& source, const int& start) {
    // fast check
    SourceView result = parse_datum(lit_pattern[0], source, start);
    if (!result)    return false;

    // check the rest
    int position = start;

    for (auto p = lit_pattern.begin() + 1;   p != lit_pattern.end;   ++p) {
        SourceView result = parse(*p, source, position);
        if (!result)    return false;

        position = result.end;
    }

    return {start, position};
}


SourceView parse_any (const std::initializer_list<Datum>& patterns, const std::string& source, const int& start) {
    for (auto& pattern : patterns) {
        SourceView result = parse(pattern, source, start);
        if (result)    return result;
    }

    return false;
}


DatumView parse_seq (const std::initializer_list<Datum>& patterns, const std::string& source, const int& start) {
    // fast check
    SourceView result = parse(*(patterns.begin()), source, start);
    if (!result)    return false;
    
    // check the rest
    int position = start;

    for (auto p = patterns.begin() + 1;   p != patterns.end();   ++p) {
        SourceView result = parse(*p, source, position);
        if (!result)    return false;

        position = result.end;
    }

    return {start, position};
}


// ---------------------------------------------------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------------------------------------------------







int main (int, char**) {
    std::string text = "print(\"Hello, World!\n\");";
    std::cout << "Source: " << text << "\n";


    // Direct use
    lit open       = {"print("};
    lit close      = {")"};
    any body       = {"Hello, World!", "Goodnight, Moon!"};
    lit terminator = ";"
    seq print_str  = {open, body, close, terminator};

    print_str(text);

    std::cout << "\n1. Direct calls:\n";
    std::cout << print_str.to_string() << ";    match = (" << print_str.start << ", " << print_str.end << ") \"" << print_str.get_match(text) << "\"\n";
    std::cout << print_str[0].to_string() << "; match = (" << print_str[0].start << ", " << print_str[0].end << ") \"" << print_str[0].get_match(text) << "\"\n";
    std::cout << print_str[1].to_string() << "; match = (" << print_str[1].start << ", " << print_str[1].end << ") \"" << print_str[1].get_match(text) << "\"\n";
    std::cout << print_str[2].to_string() << "; match = (" << print_str[2].start << ", " << print_str[2].end << ") \"" << print_str[2].get_match(text) << "\"\n";
    std::cout << "Type: " << typeid(print_str).name() << "\n";


     // Concept
    Parser open2       = lit {"print("};
    Parser close2      = lit {")"};
    Parser body2       = any {"Hello, World!", "Goodnight, Moon!"};
    Parser terminator2 = lit(";")
    Parser print_str2  = seq {open2, body2, close2, terminator2};

    print_str2(text);

    std::cout << "\n1. Direct calls:\n";
    std::cout << print_str2.to_string() << ";    match = (" << print_str2.start << ", " << print_str2.end << ") \"" << print_str.get_match(text) << "\"\n";
    std::cout << print_str2[0].to_string() << "; match = (" << print_str2[0].start << ", " << print_str2[0].end << ") \"" << print_str2[0].get_match(text) << "\"\n";
    std::cout << print_str2[1].to_string() << "; match = (" << print_str2[1].start << ", " << print_str2[1].end << ") \"" << print_str2[1].get_match(text) << "\"\n";
    std::cout << print_str2[2].to_string() << "; match = (" << print_str2[2].start << ", " << print_str2[2].end << ") \"" << print_str2[2].get_match(text) << "\"\n";
    std::cout << "Type: " << typeid(print_str2).name() << "\n";
}



#endif