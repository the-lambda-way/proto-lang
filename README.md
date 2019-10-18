# Proto Language
(Note: this language is still in its initial stage, with various techniques being explored.)

Proto is on a mission to decentralize programming from the monolithic languages we currently are forced to operate within. Too much attention is spent on mastering the idiosyncracies and constraints of existing languages, when we should be focused solely on the considerations of the domains under consideration.

Proto divides this problem into two aspects: syntax conventions and execution models.

I propose the radical view that syntax should not be defined for us, but that conventions should arise naturally out of common usage, just as they do within science, engineering, and in natural language. This is already the situation in API design - Proto would simply extend this to syntax usage as well. Language-oriented programming environments like Racket and JetBrains MPS currently have a lot to offer in this area, but I think the situation can be pushed even further. Working with syntax extensions should be as easy as working with functions or objects. I consider Racket to be the forerunner in this area, but it is currently anchored to its lisp roots. A newly announced effort by Matthew Flatt, dubbed Racket2 (<https://github.com/racket/racket/wiki/Racket2>), is a step in this direction, and worth keeping an eye on.

Execution models govern both the capabilities of a programming language, as well as the semantics which are most convenient to use. It is harder to envision how to put control of these in the hands of language users in an accessible way, but for a start I think it is worth exposing a representative set of small components with formally-defined semantics, that might be reflective of a compiler-building toolbox with a strong emphasis on compositional design. Ideally, it would be possible to move between functional semantics and stateful semantics (for example) in the same program along well-defined boundaries, without incidental performance penalty or complexity, and without relying on ad-hoc language integration. The same way that Racket users can declare a #lang to work with, Proto users could declare a paradigm or execution environment.

If you feel like chatting, feel free to stop on by the Discord server (https://discord.gg/KxD2U4f).



# Current Progress

*10/18/2019*

Since syntax manipulation is a core part of the language, it is important to have a robust set of parsing tools to build upon. The beginning of these tools are found in the *pattern* folder (working name of the parser tools library).

I've been using the Lox language (*http://www.craftinginterpreters.com*) as a test bed for the library's features. An implementation of the language in plain C++ is found in the file *lox-text.cpp*. At this point it tokenizes a portion of the language (up to chapter 4 of the *Crafting Interpreters* book).

A goal of this library is to be widely applicable while remaining useable. I've identified four levels of abstraction which I think provide a high degree of flexibility, and assists in incrementally improving code over time. These are

* *low-level*, which requires explicitly handling every customization point or parameter of a function, as well as memory layout;
* *mid-level*, which combines existing algorithms and predefined structures or other language constructs to compose useful abstractions;
* *high-level*, which uses declarative-style code with minimal manual memory management; and
* *domain-level*, which is often supplied as a DSL imposing a set of constraints which isolate contextually meaningful semantics, and providing improved syntax.

These four layers are explored below with examples, which will serve as the initial introduction to the Pattern library.



# Pattern Library

These examples demonstrate scanning and tokenizing a *number*, which is defined as either an *integer* or a *decimal*. An integer here is defined as a sequence of digits, and a decimal as an integer, followed by '.', followed by an integer. We'll also showcase incrementally tokenizing both integers and decimals in the same function.

The following definitions are used throughout the examples:

```c++
#include <string>
#include <variant>
#include "../include/scanning-algorithms.h"
#include "../include/scanner.h"

enum class TokenType    { INTEGER, DECIMAL, NONE };
using number_token = token<TokenType, std::variant<std::monostate, int, double>>;

number_token none_token {TokenType::NONE, std::monostate {}};
constexpr bool is_digit (char c)    { return '0' <= c && c <= '9'; }

std::string to_string (std::string_view s)    { return std::string {s.data(), s.length()}; }
```



## Low-level

A *scanner* type, which is provided in *scanner.h*, combines a string view and iterator to manage the scanning of source code using traditional iterator semantics. It can be included as part of a custom parser, or used separately and passed between functions.



```c++
number_token number (scanner& s)
{
    if (!is_digit(*s))    return none_token;

    // Integer
    std::string match = "" + *s++;

    while (is_digit(*s))    match += *s++;

    if (*s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(match)};

    // Decimal
    match += *s++ + *s++;

    while (is_digit(*s))    match += *s++;

    return {TokenType::DECIMAL, std::stod(match)};
}

// Add number to your custom scanner
```



