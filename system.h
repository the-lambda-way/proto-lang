#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include "syntax.h"
using namespace std;


namespace System {
extern bool in_error;
extern string current_source;
void report (string msg, string code, Syntax::SourceLoc location);
void fail (string msg, string code, Syntax::SourceLoc location);
}


#endif

