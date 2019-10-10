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


struct file_position;

// path should maybe change to a shared smart file pointer at some point, since source code metadata is transient in nature.
struct source_location
{
    const char* path  = "";
    const int   index = 0;
    const int   span  = 0;

    file_position position  ();
    int           line      ();
    int           column    ();
    std::string   to_string ();
};


struct file_position
{
    int line   = 0;
    int column = 0;

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
    }

    file_position (const source_location& s) : file_position {s.path, s.index} {}
};


file_position source_location::position  ()    { return {path, index};     }
int           source_location::line      ()    { return position().line;   }
int           source_location::column    ()    { return position().column; }
std::string   source_location::to_string ()    { return file_to_string(path, index, span); }


template <typename ObjectType, typename ValueType>
class token
{
public:
    ObjectType type;
    ValueType  literal;

    constexpr token (ObjectType type)                    : type {type}, literal {""} {}
    constexpr token (ObjectType type, ValueType literal) : type {type}, literal {literal} {}
};


template <typename ObjectType, typename ValueType>
struct token_loc
{
    const ObjectType type;
    const ValueType  literal;
    source_location  location;

    constexpr file_position postion ()    { return location.position();  }
    constexpr int           line    ()    { return location.line();      }
    constexpr int           column  ()    { return location.column();    }
              std::string   lexeme  ()    { return location.to_string(); }

    constexpr token_loc (ObjectType type)
        : type {type}, literal {""}, location {}
    {}

    constexpr token_loc (ObjectType type, ValueType literal)
        : type {type}, literal {literal}, location {}
    {}

    constexpr token_loc (ObjectType type, ValueType literal, source_location location)
        : type {type}, literal {literal}, location {location}
    {}
};


#endif // SYNTAX
