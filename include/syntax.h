#ifndef SYNTAX_H
#define SYNTAX_H

#include <string>
#include <vector>


/***********************************************************************************************************************
 * Syntax Library
 * The Syntax Library provides abstraction mechanisms for inspecting and manipulating source code.
 * 
 **********************************************************************************************************************/

namespace Syntax {


struct SourceLoc {
    const std::string origin   = "";
    const int         position = 0;
    const int         line     = 0;
    const int         column   = 0;
    const int         span     = 0;
};


struct SyntaxObject {
    const std::string name;
    const std::string lexeme;
    const SourceLoc   location;
};


template<class ValueType>
struct SemanticObject {
    const std::string  name;
    const ValueType    value;
    const SyntaxObject ancestor;
};


// SourceCode represents an immutable body of code, providing metadata.
class SourceCode {
public:
    SourceCode (std::string origin, std::string source);

          int         pos      = 0;
    const int         src_end;
    const std::string source;
    const std::string origin;
    
    const std::string & operator[] (int position) const;

    SourceLoc   get_location (int position);
    std::string get_line     (int line);
};


} // namespace Syntax


#endif