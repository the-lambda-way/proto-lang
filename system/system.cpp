#include "system.h"
#include <iostream>    // cout
#include "syntax.h"
using namespace Syntax;


bool System::in_error = false;


void System::report (string msg, string code, SourceLoc location) {
    string line_num = to_string(location.line);
    string arrow (line_num.length() + location.column, ' ');
    arrow.append("--^--");

    cout << "Error: " << msg << endl
         << line_num << " | " << code << endl
         << arrow << endl
         << "In " << location.line << "::" << location.column << "::" << location.source << endl;}


void System::fail (string msg, string code, SourceLoc location) {
    report(msg, code, location);
    in_error = true;}