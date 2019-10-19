#ifndef SYNTAX
#define SYNTAX

#include <fstream>    // get_file_contents
#include <string>


/***********************************************************************************************************************
 * Syntax Library
 * The Syntax Library provides abstractions for inspecting and managing source code.
 *
 **********************************************************************************************************************/


struct source_location
{
    const size_t position;
    const size_t span;

    template <typename CharT>
    constexpr std::basic_string_view<CharT> lexeme (CharT* data)    { return {data + position, span}; }


    constexpr source_location (size_t position, size_t span)
        : position {position}, span {span}
    {}


    template <typename CharT>
    constexpr source_location (CharT* data, CharT* start, size_t span)
        : position {start - data}, span {span}
    {}


    template <typename CharT>
    constexpr source_location (CharT* data, CharT* start, CharT* end)
        : position {start - data}, span {end - start}
    {}


    template <typename CharT>
    constexpr source_location (CharT* data, std::basic_string_view<CharT> lexeme)
        : position {lexeme.begin() - data}, span {lexeme.length()}
    {}


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, CharT* start, size_t span)
        : position {start - source.begin()}, span {span}
    {}


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, CharT* start, CharT* end)
        : position {start - source.begin()}, span {end - start}

    {}


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, std::basic_string_view<CharT> lexeme)
        : position {lexeme.begin() - source.begin()}, span {lexeme.length()}
    {}
};


// based on http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents (const std::string& path, size_t index = 0, size_t span = -1)
{
    using namespace std;

    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)   throw (errno);

    // Allocate string memory
    std::string contents;

    span = std::min(span, (size_t) file.tellg() - index);
    contents.resize(span);

    // Read file contents into string
    file.seekg(index);
    file.read(contents.data(), span);

    return contents;
}


std::string get_file_contents (const std::string& path, source_location s)
{
    return get_file_contents (path, s.position, s.span);
}


struct file_position
{
    int line;
    int column;


    constexpr file_position (int line, int column)
        : line {line}, column {column}
    {}


    template <typename CharT>
    constexpr file_position (const CharT* data, const CharT* position)
    {
        convert_from(data, position);
    }


    template <typename CharT>
    constexpr file_position (const CharT* data, size_t position)
    {
        convert_from(data, data + position);
    }


    template <typename CharT>
    constexpr file_position (const CharT* data, std::basic_string_view<CharT> lexeme)
    {
        convert_from(data, lexeme.data());
    }


    template <typename CharT>
    constexpr file_position (const CharT* data, source_location location)
    {
        convert_from(data, data + location.position);
    }


    template <typename CharT>
    constexpr file_position (std::basic_string_view<CharT> source, CharT* position)
    {
        convert_from(source.data(), position);
    }


    template <typename CharT>
    constexpr file_position (std::basic_string_view<CharT> source, size_t position)
    {
        convert_from(source.data(), source.data() + position);
    }


    template <typename CharT>
    constexpr file_position (std::basic_string_view<CharT> source, std::basic_string_view<CharT> lexeme)
    {
        convert_from(source.data(), lexeme.data());
    }


    template <typename CharT>
    constexpr file_position (std::basic_string_view<CharT> source, source_location location)
    {
        convert_from(source.data(), source.data() + location.position);
    }


    file_position (const char* path, int position)
    {
        std::string source = get_file_contents(path, 0, position);
        convert_from(source.data(), source.data() + position);
    }


private:
    template <typename CharT>
    constexpr void convert_from (const CharT* data, const CharT* position)
    {
        line = 1;
        auto mark = data;

        for (auto i = data;    i != position;    ++i)
        {
            if (*i == '\n')
            {
                ++line;
                mark = i;
            }
        }

        column = position - mark;
    }
};


template <typename TagType, typename ValueType>
class token
{
public:
    TagType   tag;
    ValueType value;

    constexpr token (TagType tag)                    : tag {tag}, value {""} {}
    constexpr token (TagType tag, ValueType literal) : tag {tag}, value {value} {}
};


// Holds a reference to source in order to view its lexeme and construct metadata when needed.
// Recommended for use where access to token metadata is rare.
template <typename TagType, typename ValueType, typename CharT = char>
struct token_lex
{
    const TagType   tag;
    const ValueType value;
    const std::basic_string_view<CharT> lexeme;

    constexpr token_lex (TagType tag)
        : tag {tag}, value {}, lexeme {}
    {}

    constexpr token_lex (TagType tag, ValueType value)
        : tag {tag}, value {value}, lexeme {}
    {}

    constexpr token_lex (TagType tag, ValueType value, std::basic_string_view<CharT> lexeme)
        : tag {tag}, value {value}, lexeme {lexeme}
    {}

    constexpr size_t          position      (const CharT* data) const    { return lexeme.data() - data; }
    constexpr size_t          span          ()                  const    { return lexeme.length();      }
    constexpr source_location location      (const CharT* data) const    { return {data, lexeme};       }
    constexpr ::file_position file_position (const CharT* data) const    { return {data, lexeme};       }
};



// Need to clean this up

// Calculates and holds all metadata upon its construction.
// Recommended for use where access to token metadata is frequent.
// template <typename TagType, typename ValueType, typename CharT = char>
// struct token_loc
// {
//     const TagType         tag;
//     const ValueType       value;
//     const source_location location;
//     const ::file_position file_position;
//     const CharT*          data;
//     const std::string     origin;


//     constexpr size_t position ()    { return location.position;    }
//     constexpr size_t span     ()    { return location.span;        }
//     constexpr int    line     ()    { return file_position.line;   }
//     constexpr int    column   ()    { return file_position.column; }

//     constexpr std::basic_string_view<CharT> lexeme ()    { return {data + position(), span()}; }


//     // Probably need to add more constructors, maybe create a separate token_information object


//     constexpr token_loc () = default;

//     constexpr token_loc (TagType tag, ValueType value, CharT* data, CharT* start, CharT* end, std::string origin)
//         : tag {tag}, value {value}, location {data, start, end}, file_position {data, start}, data {data}, origin {origin}
//     {}

//     constexpr token_loc (token_lex<TagType, ValueType, CharT> t, CharT* data, std::string origin)
//         : token_loc {t.tag, t.value, data, t.lexeme.data(), t.data() + t.length(), origin}
//     {}
// };


#endif // SYNTAX
