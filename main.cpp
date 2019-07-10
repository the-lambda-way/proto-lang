#include <fstream>     // get_file_contents
#include <iostream>    // repl
#include <string>
#include "syntax.h"
#include "system.h"
using namespace std;
using namespace Syntax;


// source: http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
string get_file_contents (string path) {
    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)    throw (errno);

    // Create string of sufficient size
    string contents;
    contents.resize(file.tellg());

    // Read complete file into string
    file.seekg(0, ios::beg);
    file.read(&contents[0], contents.size());

    // Wrapup
    file.close();
    return(contents);
}


void run (SourceCode code) {
    // Just print code for now
    cout << CodeProcessor(code).expand().source << endl;
}


void repl () {
    string line;
    
    for (;;) {
        cout << "> ";

        getline(cin, line);
        run(SourceCode("repl", line));
        
        if (System::in_error)    exit(EXIT_FAILURE);
    }
}


void run_file (string path) {    
    string code = get_file_contents(path);
    run(SourceCode(path, code));
    
    if (System::in_error)    exit(EXIT_FAILURE);
}


int main (int argc, char* argv[]) {
    try {
             if (argc >  2)    cout << "Usage: proto [script]" << endl;
        else if (argc == 2)    run_file(argv[0]);
        else                   repl();
    }
    catch (exception& e) {
        cerr << e.what() << endl;
    }
}