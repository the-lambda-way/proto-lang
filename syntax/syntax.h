#ifndef SYNTAX_H
#define SYNTAX_H

#include <string>
#include <vector>
using namespace std;


/***********************************************************************************************************************
 * Syntax Library
 * The Syntax Library provides abstraction mechanisms for inspecting and manipulating source code.
 * 
 **********************************************************************************************************************/

// Eventually, this library should parse code as a human would, given a respectable amount of formality in the grammar.
//     That will require extended study, so in the meantime, it starts with a managable simplification of the process.

// By default, code is processed left-to-right. Every construct manages the code that follows it. The construct is
//     required to return a new syntax object, as well as the amount of characters that were processed, so that the
//     parser can replace the processed syntax and continue parsing from that point.
// Within that environment, special parsing patterns may be defined. One example is a binary operator. When special
//     parsers are finished, they should have reorganized the code in left-to-right form.
// Parsers can yield to other parsers when they encounter an unknown object that may be compatible with their expected
//     forms.


namespace Syntax {


struct SourceLoc {
    const string origin   = "";
    const int    position = 0;
    const int    line     = 0;
    const int    column   = 0;
};


struct SyntaxObject {
    const void*  token;         // specialized by the parsing object
    const string lexeme;
    const int    position;
};


// SourceCode represents an immutable body of code, providing metadata.
class SourceCode {
public:
    SourceCode(string origin, string source);

          int    pos = 0;
    const int    src_end;
    const string source;
    const string origin;
    
    const string & operator[] (int position) const;

    SourceLoc get_location (int position);
    string    get_line     (int line);
};


// ---------------------------------------------------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------------------------------------------------
// A Macro receives a substring of source code and returns a SyntaxObject. It is assumed the code has the right
//     pattern and the correct length.
class Macro {
public:
    Macro (void (*impl)(std::string, int));
    SyntaxObject operator() (std::string code, int pos);
private:
    void (*impl)(std::string, int);
};



// ---------------------------------------------------------------------------------------------------------------------
// Code Transformations
// ---------------------------------------------------------------------------------------------------------------------
// A CodeProcessor manages syntax transformations over a body of code.
class CodeProcessor {
public:
    SourceCode source;
    CodeProcessor(SourceCode source);

    void add(Macro macro);
    SourceCode expand();

private:
    SourceCode code;
    int        index;
    int        indent;
};


} // namespace Syntax

#endif