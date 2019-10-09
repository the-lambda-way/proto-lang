#ifndef SYNTAX
#define SYNTAX

#include <fstream>    // get_file_contents
#include <string>
#include <tuple>      // std::tie


// Down the road, std::string functions should be replaced with a buffered stream for more efficient parsing.


// based on http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string file_to_string (std::string path, size_t index = 0, size_t span = -1)
{
    using namespace std;

    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)   throw (errno);

    // Adjust span if needed
    span = std::min(span, (size_t) file.tellg() - index);

    // Allocate string memory
    std::string contents;
    contents.resize(span);

    // Read file contents into string
    file.seekg(index);
    file.read(&contents[0], span);

    return contents;
}


// path should maybe change to a shared smart file pointer at some point, since source code metadata is transient in nature.
struct source_location
{
    const char* path;
    const int   index;
    const int   span;

    constexpr file_position position  ()    { return {path, index}; }
    constexpr int           line      ()    { return position().line;   }
    constexpr int           column    ()    { return position().column; }
              std::string   to_string ()    { return file_to_string(path, index, span); }
};


struct file_position
{
    int line;
    int column;

    constexpr file_position (int line, int column) : line {line}, column {column} {}

    file_position (const char* path, int index)
    {
        std::string contents = file_to_string(path, 0, index);

        line = 1;
        auto begin = contents.begin();
        auto mark  = begin;

        for (auto i = begin;    i != contents.end();    ++i)
            if (*i == '\n')
            {
                ++line;
                mark = i;
            }

        column = index - (mark - begin) - 1;
    }

    file_position (const source_location& s) : file_position {s.path, s.index} {}
};



template <typename ObjectType, typename ValueType>
struct syntax_object
{
    const ObjectType type;
    const ValueType  literal;
};

template <typename ObjectType, typename ValueType>
struct syntax_object_loc
{
    const ObjectType      type;
    const ValueType       literal;
    const source_location location;

    constexpr file_postion postion ()    { return location.position(); }
    constexpr int          line    ()    { return location.line();     }
    constexpr int          column  ()    { return location.column();   }
              std::string  lexeme  ()    { return location.to_string(); }
};


#endif // SYNTAX
