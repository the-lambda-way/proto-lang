#include "pattern.h"
#include <limits>
using namespace PatLib;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------
// Primitive types
// ---------------------------------------------------------------------------------------------------------------------

// lit
// -------------------------
lit::lit (char        value) : value(std::string(1, value)), size(1)               {}
lit::lit (std::string value) : value(value),                 size(value.size())    {}

bool lit::operator() (std::string source, int pos) {
    // fast checks
    if (value[0] != source[pos])       return false;    // usual result
    if (source.size() - pos < size)    return false;    // faster than checking every loop

    // check the rest
    start = pos;
    
    for (end = pos + 1;   end < start + size;   ++end)
        if (value[end - pos] != source[end])    return false;

    return true;
}


vector<Pattern&> str_to_lits (vector<std::string> strings) {
    vector<Pattern&> lits;
    lits.reserve(strings.size());

    for (std::string s : strings) {
        lits.push_back(lit(s));
    }

    return lits;
}


// any
// -------------------------
any::any (char a[], char b[])               : any(str_to_lits({a, b}))       {}
any::any (vector<Pattern&>    patterns)     : patterns(patterns)             {}
any::any (vector<std::string> lit_patterns) : any(str_to_lits(lit_patterns)) {}

bool any::operator() (std::string source, int pos) {
    start = pos;

    for (which = 0;   which < patterns.size();   ++which) {
        if (patterns[which](source, pos)) {
            end = patterns[which].end;
            return true;
        }
    }

    return false;
}


// seq
// -------------------------
seq::seq (char a[], char b[])        : seq(str_to_lits({a, b})) {}
seq::seq (vector<Pattern&> patterns) : patterns(patterns)       {}

seq::seq (vector<Pattern&> patterns, Pattern& separator) {
    // interleave separators
    this->patterns.reserve(2*patterns.size() - 1);
   
    for (auto p = patterns.begin();   p < patterns.end() - 1; ++p) {
        this->patterns.push_back(*p);
        this->patterns.push_back(separator);
    }

    this->patterns.push_back(patterns.back());
}

seq::seq (vector<std::string> lit_patterns)                     : seq(str_to_lits(lit_patterns))            {}
seq::seq (vector<std::string> lit_patterns, Pattern& separator) : seq(str_to_lits(lit_patterns), separator) {}

bool seq::operator() (std::string source, int pos) {
    // fast check
    if (!patterns[0](source, pos))    return false;

    // check the rest
    start = pos;

    for (Pattern& pattern : patterns) {
        if (!pattern(source, pos))    return false;
        pos = pattern.end;
    }

    end = patterns.back().end;
    return true;
}


// until
// -------------------------
until::until (Pattern& pattern) : pattern(pattern) {}

bool until::operator() (std::string source, int pos) {
    // fast check
    if (pattern(source, pos))    return false;

    // search for pattern
    start = pos;

    for (end = pos + 1;   end < source.size();   ++end) {
        if (pattern(source, pos)) {
            end = pattern.start;
            break;
        }
    }

    return true;
}


// rep
// -------------------------
rep::rep (Pattern& pattern)        : rep(pattern, 0, -1) {}
rep::rep (Pattern& pattern, int n) : rep(pattern, n, n)  {}

rep::rep (Pattern& pattern, int min, int max)
    : pattern(pattern), min(min), max(max) {
    if (max == -1)    max = std::numeric_limits<int>::max();
}

bool rep::operator() (std::string source, int pos) {
    // fast check
    if (min > 0) {
        if (!pattern(source, pos))    return false;
    }
    
    start      = pos;
    matches[0] = pattern;
    pos        = pattern.end;

    // keep repeating
    for (int i = 1;   i < max;   ++i) {
        if (!pattern(source, pos)) {
            if   (i < min)    { return false; }
            else              { break; }
        }

        matches[i] = pattern;
        pos        = pattern.end;
    }

    // finished
    end    = matches.back().end;
    amount = matches.size();
    return true;
}
    
