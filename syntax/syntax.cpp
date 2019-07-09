#include "syntax.h"
#include <limits>           // numeric_limits
#include "system.h"         // System::fail()

using namespace std;
using namespace Syntax;


// ---------------------------------------------------------------------------------------------------------------------
// SourceCode
// ---------------------------------------------------------------------------------------------------------------------

SourceCode::SourceCode (string origin, string source)
    : origin(origin), source(source), src_end(source.size()) {}


const string& SourceCode::operator[] (int position) const    { return source[position]; }


SourceLoc SourceCode::get_location (int position) {
    SourceLoc location;

    if (position > src_end) {
        location = get_location(pos);

        System::fail("Position " + to_string(position) + " is past the end of the source.",
                     get_line(location.line),
                     location);
        
        return;}

    
    location.origin   = origin;
    location.position = position;


    // Find line number
    location.line = 0;
    int start = 0;

    for (int i = 0; i < position; ++i) {
        if (source[i] == "\n") {
            ++location.line;
            start = i + 1;}}
    
    
    location.column = position - start;
    
    return location;}


string SourceCode::get_line (int line) {
    int line_num, start, end;
    line_num = start = end = 0;
    
    while (line_num < line && start < src_end) {
        if (source[start] == "\n")    ++line_num;
        ++start;}


    if (start < src_end) {
        end = start;
        while (source[end] != "\n" && end < src_end)    ++end;

        return source.substr(start, end);}


    // line not found
    SourceLoc location = get_location(pos);

    System::fail("Could not find the line " + line + " in " + origin,
                    get_line(location.line),
                    location);
    
    return "";}




// ---------------------------------------------------------------------------------------------------------------------
// Syntax transformers
// ---------------------------------------------------------------------------------------------------------------------




// ---------------------------------------------------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------------------------------------------------

