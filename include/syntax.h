#ifndef SYNTAX
#define SYNTAX

#include <fstream>    // get_file_contents
#include <string>
#include <tuple>      // std::tie


// source: http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents (std::string path) {
    using namespace std;

    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)    throw (errno);

    // Create string of sufficient size
    std::string contents;
    contents.resize(file.tellg());

    // Read complete file into string
    file.seekg(0, ios::beg);
    file.read(&contents[0], contents.size());

    return contents;
}


// Returns a pair (line, index) of the current line and the index it starts at, relative to "first".
template <typename CharT>
constexpr std::pair<int, int> count_lines (const CharT* first, const CharT* last)
{
    int line  = 1;
    int index = 0;

    for (auto i = first;    i != last;    ++i)
        if (*i == '\n')
        {
            ++line;
            index = i - first;
        }

    return {line, index};
}


struct source_location
{
    const char* path;
    const int   index;

    file_position position ()    { return {path, index}; }
    int line   ()    { return position().line;   }
    int column ()    { return position().column; }
};


struct file_position
{
    int line;
    int column;

    constexpr file_position (int line, int column) : line {line}, column {column} {}

    constexpr file_position (const char* path, int index)
    {
        string_view contents   = get_file_contents(path);
        std::tie(line, column) = count_lines(contents.begin(), &contents[index]);
        column = index - column - 1;
    }

    constexpr file_position (const source_location& s) : file_position {s.path, s.index} {]
};


#endif // SYNTAX
