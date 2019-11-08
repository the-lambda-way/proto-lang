/**
 * @file
 * @author Mike Castillo
 *
 * @section License
 *
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * @section Description
 *
 * Facilities for managing source code.
 */

#ifndef SYNTAX
#define SYNTAX

#include <fstream>    // file_to_string
#include <string>


/**
 * A position and span, designed for character sequences.
 */
struct source_position
{
    size_t position;
    size_t span;

    template <typename CharT>
    constexpr std::basic_string_view<CharT> lexeme (CharT* data)    { return {data + position, span}; }


    constexpr source_position (size_t position, size_t span)
        : position {position}, span {span}
    {}


    template <typename CharT>
    constexpr source_position (CharT* data, CharT* start, size_t span)
        : position {start - data}, span {span}
    {}


    template <typename CharT>
    constexpr source_position (CharT* data, CharT* start, CharT* end)
        : position {start - data}, span {end - start}
    {}


    template <typename CharT>
    constexpr source_position (CharT* data, std::basic_string_view<CharT> lexeme)
        : position {lexeme.begin() - data}, span {lexeme.length()}
    {}


    template <typename CharT>
    constexpr source_position (std::basic_string_view<CharT> source, CharT* start, size_t span)
        : position {start - source.begin()}, span {span}
    {}


    template <typename CharT>
    constexpr source_position (std::basic_string_view<CharT> source, CharT* start, CharT* end)
        : position {start - source.begin()}, span {end - start}

    {}


    template <typename CharT>
    constexpr source_position (std::basic_string_view<CharT> source, std::basic_string_view<CharT> lexeme)
        : position {lexeme.begin() - source.begin()}, span {lexeme.length()}
    {}
};


/**
 * Retrieve the contents of a file into a string.
 *
 * Based on http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
 *
 * @param    path    Pathname of the file to open
 * @param    start   Index to begin retrieving from
 * @param    span    Number of characters to retrieve
 * @return   Contents retrieved from file
 */
std::string file_to_string (const std::string& path, size_t start = 0, size_t span = -1)
{
    using namespace std;

    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)   throw (errno);

    // Allocate string memory
    std::string contents;

    span = std::min(span, (size_t) file.tellg() - start);
    contents.resize(span);

    // Read file contents into string
    file.seekg(start);
    file.read(contents.data(), span);

    return contents;
}


/**
 * Retrieve the contents of a file into a string.
 *
 * @param    path   Pathname of the file to open
 * @param    s      Position and span to retrieve from
 * @return   Contents retrieved from file
 */
std::string file_to_string (const std::string& path, source_position s)
{
    return file_to_string (path, s.position, s.span);
}


/**
 * A line and column, designed for character sequences.
 */
struct source_location
{
    int line;
    int column;


    constexpr source_location (int line, int column)
        : line {line}, column {column}
    {}


    template <typename CharT>
    constexpr source_location (const CharT* data, const CharT* position)
    {
        convert_from(data, position);
    }


    template <typename CharT>
    constexpr source_location (const CharT* data, size_t position)
    {
        convert_from(data, data + position);
    }


    template <typename CharT>
    constexpr source_location (const CharT* data, std::basic_string_view<CharT> lexeme)
    {
        convert_from(data, lexeme.data());
    }


    template <typename CharT>
    constexpr source_location (const CharT* data, source_position srcpos)
    {
        convert_from(data, data + srcpos.position);
    }


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, CharT* position)
    {
        convert_from(source.data(), position);
    }


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, size_t position)
    {
        convert_from(source.data(), source.data() + position);
    }


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, std::basic_string_view<CharT> lexeme)
    {
        convert_from(source.data(), lexeme.data());
    }


    template <typename CharT>
    constexpr source_location (std::basic_string_view<CharT> source, source_position srcpos)
    {
        convert_from(source.data(), source.data() + srcpos.position);
    }


    source_location (const char* path, int position)
    {
        std::string source = file_to_string(path, 0, position);
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


/**
 * A simple token structure.
 */
template <typename TagType, typename ValueType>
struct token
{
    TagType   tag;
    ValueType value;

    constexpr token (TagType tag)                    : tag {tag}, value {""} {}
    constexpr token (TagType tag, ValueType literal) : tag {tag}, value {value} {}
};


/**
 * A token combined with its lexeme, which can be used to construct location information.
 *
 * Metadata can be constructed when needed, so is recommended for use where access to token metadata is rare. The
 * original source pointer must be supplied to calculate this information.
 */
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

    constexpr size_t            position        (const CharT* data) const    { return lexeme.data() - data; }
    constexpr size_t            span            ()                  const    { return lexeme.length();      }
    constexpr ::source_position source_position (const CharT* data) const    { return {data, lexeme};       }
    constexpr ::source_location source_location (const CharT* data) const    { return {data, lexeme};       }
};



// Need to clean this up

// Calculates and holds all metadata upon its construction.
// Recommended for use where access to token metadata is frequent.
// template <typename TagType, typename ValueType, typename CharT = char>
// struct token_loc
// {
//     const TagType         tag;
//     const ValueType       value;
//     const source_position location;
//     const ::source_location source_location;
//     const CharT*          data;
//     const std::string     origin;


//     constexpr size_t position ()    { return location.position;    }
//     constexpr size_t span     ()    { return location.span;        }
//     constexpr int    line     ()    { return source_location.line;   }
//     constexpr int    column   ()    { return source_location.column; }

//     constexpr std::basic_string_view<CharT> lexeme ()    { return {data + position(), span()}; }


//     // Probably need to add more constructors, maybe create a separate token_information object


//     constexpr token_loc () = default;

//     constexpr token_loc (TagType tag, ValueType value, CharT* data, CharT* start, CharT* end, std::string origin)
//         : tag {tag}, value {value}, location {data, start, end}, source_location {data, start}, data {data}, origin {origin}
//     {}

//     constexpr token_loc (token_lex<TagType, ValueType, CharT> t, CharT* data, std::string origin)
//         : token_loc {t.tag, t.value, data, t.lexeme.data(), t.data() + t.length(), origin}
//     {}
// };


#endif // SYNTAX
