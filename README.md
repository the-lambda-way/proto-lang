# Proto Language

(Note: this language is still in its initial stage, with various techniques being explored.)

Proto is a framework on a mission to decentralize programming from the monolithic syntactic and semantic systems that programming languages traditionally operate under. Currently, learning a programming language means learning a whole new set of notational conventions (hopefully similar to ones you are familiar with!) which are tightly coupled to a bespoke execution model. As a result, too much attention is spent on mastering the idiosyncrasies and constraints of a language, when we should be focused solely on the considerations of the domains we choose, using whatever syntax is most convenient.

Instead of syntax being constrained for us by our tools, conventions should arise naturally out of common usage, just as they do within science, engineering, and in natural language. This is already the situation in API design (within the constraints of a language's paradigms), and Proto would extend this to syntax usage as well. Working with syntax extensions should be as easy as working with functions or objects.

Likewise with execution models. Users should be able to move between paradigms without incidental performance penalty or complexity, and without relying on ad-hoc language integration. Choosing an execution model or paradigm should be as easy as defining a new module.

If you feel like chatting, feel free to stop on by the Discord server (https://discord.gg/KxD2U4f).



# Design

Since syntax manipulation is a core part of the language, it is important to have a robust set of parsing tools to build upon. The Pattern library is being developed as a user-friendly toolkit for building recursive-descent parsers. Because Proto is meant to be more of a framework than a traditional language, every effort is made to adapt the C++ implementation to C++ conventions. But once it is developed to a sufficient point, the library will be used to bootstrap the Proto language proper, which should be much more flexible in developing the stronger features of the library (such as lisp-macros). I intend to continue developing the C++ library alongside the Proto version, matching the feature set as much as possible.

Interpretation of Proto's notation will serve as the test case for an initial procedural execution model included in the language. Once a rudimentary language runs, exploration of other execution models / paradigms can begin.



# Pattern Library Quick Look

Below are various ways in which you might tokenize a number which can be either an integer or a decimal. Assume that the current character has been identified as a digit in your outer loop.



## Set up

The following definitions are used throughout the examples:

```c++
#include <string>
#include <string_view>
#include <variant>
#include "scan_view.h"
#include "scanning-algorithms.h"
#include "syntax.h"

enum class TokenType { INTEGER, DECIMAL };
using number_token = token<TokenType, std::variant<int, double>>;

constexpr bool is_digit (char c)     { return '0' <= c && c <= '9'; }

std::string to_string (std::string_view s)     { return std::string {s.data(), s.length()}; }
```



## Using pointers

```c++
number_token number (scan_view& s)
{
     // Integer
     std::string match = "" + *s++;

     while (is_digit(*s))    match += *s++;

     if (s[0] != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(match)};

     // Decimal
     match += *s++ + *s++;

     while (is_digit(*s))    match += *s++;

     return {TokenType::DECIMAL, std::stod(match)};
}
```



## Using algorithms

```c++
number_token number (scan_view& s)
{
     s.save();

     // Integer
     ++s;
     advance_while(s, is_digit);

     if (*s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};

     // Decimal
     s += 2;
     advance_while(s, is_digit);

     return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}
```



## Using higher-order functions

```c++
number_token number (scan_view& s)
{
     constexpr auto integer    = Scan::some(is_digit);
     constexpr auto fractional = Scan::join('.', integer);

     s.save();

     integer(s);

     if (!fractional(s))     return {TokenType::INTEGER, std::stoi(s.copy_skipped())};
     return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}
```



## Using declarative features

(Not yet implemented.)

```c++
auto integer    = Scan::some(is_digit);
auto fractional = Scan::join('.', integer);

number_token tokenize_int (std::string_view match)
{
     return {TokenType::INTEGER, std::stoi(to_string(match))};
};

number_token tokenize_dec (std::string_view match)
{
     return {TokenType::DECIMAL, std::stod(to_string(match))};
};

// Tokenize::incremental takes a list of pairs of scanners and tokenizers
auto number = Tokenize::incremental({integer,    tokenize_int},
                                    {fractional, tokenize_dec});
```



## Using grammar-like features

(Not yet implemented.)

```c++
auto digit   = GrammarExp >> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
auto integer = +digit;
auto decimal = integer & '.' & integer;

number_token tokenize_int (std::string_view match)
{
     return {TokenType::INTEGER, std::stoi(match)};
};

number_token tokenize_dec (std::string_view match)
{
     return {TokenType::DECIMAL, std::stod(match)};
};

Language myLang;

// Incremental parsing is handled automatically when rules are added
myLang.add_rule(integer, tokenize_int);
myLang.add_rule(decimal, tokenize_dec);
```



## Using built-in definitions

(Not yet implemented.)

```c++
#include "pattern.h"

Language myLang;
myLang.add_rule(PatDef::integer, [](auto m) -> number_token { return {TokenType::INTEGER, std::stoi(m)}; });
myLang.add_rule(PatDef::decimal, [](auto m) -> number_token { return {TokenType::DECIMAL, std::stod(m)}; });
```


# Documentation

[Read the docs.](https://the-proto-language.readthedocs.io/en/cpp20/)



# Current Progress

*08/10/2020*

The Pattern library is being refactored to adopt the C++20 features available in GCC 10. Recent changes are taking place in the cpp20 branch, but will soon be merged back into main for continued development.

Work continues on separating the core components into independent, but composable libraries. The functional combinator library provides tools for declaratively composing impure functions. The rest of the Pattern library is being written to use this new model, instead of the old scanners which integrated this functionality into themselves.

The Pattern library will strive to be fully compatible with C++20 ranges and their associated tools.

The Lox language (*http://www.craftinginterpreters.com*) is being used as a test bed for the library's features. An implementation of the language in plain C++ is found in the file *lox-test.cpp*. At this point it tokenizes a portion of the language (up to chapter 4 of the *Crafting Interpreters* book).

The Rosetta Code lexical analyzer (*http://rosettacode.org/wiki/Compiler/lexical_analyzer*) has been added as another test bed for the language (but hasn't been committed yet). The Super Tiny Compiler (https://github.com/jamiebuilds/the-super-tiny-compiler) will be added next. Other language-related applications will be added to this list as time goes on.

A testing framework will soon be added using Catch2 (*https://github.com/catchorg/Catch2*).

Proper documentation has begin, using Sphinx with a Read The Docs theme.

