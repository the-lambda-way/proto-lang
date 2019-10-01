#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include "syntax.h"


// Jonathan Blow in his language Jai has a global structure called a "context" that stores pointers to program-wide
//     features such as a logger, allocators, error handling, threads, etc. See https://youtu.be/uZgbKrDEzAs?t=2325
// He adds a pool allocator here used for program-wide for temporary storage. An example use case is for a function to
//     return a new string without worrying about allocating on the heap or a provided location. Then the caller grabs
//     the string from that pool if they need to store it for a longer period. The developer is responsible for clearing
//     that pool of memory at an appropriate time.


namespace System {


extern bool in_error;
extern std::string current_source;
void report (std::string msg, std::string code, Syntax::SourceLoc location);
void fail (std::string msg, std::string code, Syntax::SourceLoc location);


} // namespace System


#endif