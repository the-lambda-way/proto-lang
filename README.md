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

* *low-level*, which micromanages details, is responsible for memory layout, and relies on imperative control flow mechanisms
* *high-level*, which hides details behind encapsulations of various types, control flow managed through function composition
* *declarative*, which minimizes control flow and memory management through declarative-style code, providing tailor-made objects with configurable options
* *domain-level*, which is often supplied as a DSL imposing a set of constraints which isolate contextually meaningful semantics, and providing improved syntax

These four layers are explored below with examples, which will serve as the initial introduction to the Pattern library.



# Pattern Library

These examples demonstrate scanning and tokenizing a number, which is defined as either an integer or a decimal. An integer here is defined as a sequence of digits, and a decimal as an integer, followed by '.', followed by an integer. We'll also showcase incrementally tokenizing both integers and decimals in the same function.

The following definitions are used throughout the examples:

```c++
#include <string>
#include <variant>
#include "../include/scanning-algorithms.h"
#include "../include/scan_view.h"
#include "../include/syntax.h"


enum class TokenType    { INTEGER, DECIMAL, NONE };
using number_token = token<TokenType, std::variant<std::monostate, int, double>>;

number_token none_token {TokenType::NONE, std::monostate {}};
constexpr bool is_digit (char c)    { return '0' <= c && c <= '9'; }

std::string to_string (std::string_view s)    { return std::string {s.data(), s.length()}; }
```



## Low-level

A *scanner* type, which is provided in *scan_view.h*, combines a string view and iterator to manage the scanning of source code using traditional iterator semantics. It can be included as part of a custom scanner or parser, or used separately and passed between functions.



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

// Add number to your custom parser
```



## High-level

The *save()* member of the scan_view class provides a simple mechanism to backtrack or obtain the characters of a successful parse. The first example copied matching characters one by one into a string. This example uses *s.copy_skipped()* to return the string from the last save to the current position.

Iteration has been replaced here with algorithms.



```c++
number_token number2 (scan_view& s)
{
    s.save();

    // Integer
    if (!advance_if(s, is_digit))    return none_token;
    advance_while(s, is_digit);

    if (s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};

    // Decimal
    s += 2;
    advance_while(s, is_digit);

    return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}

// add number2 to your custom parser
```



## Declarative-level

Higher-order functions can be used to generate scanning and parsing functions for you. The next example creates matchers which return an optional string_view if the match is successful.

(Implementation hasn't been pushed yet, but is mostly done.)



```c++
scanner integer    = Scan::at_least(1, is_digit);
scanner fractional = Scan::join('.', integer);

number_token number4 (scan_view& s)
{
    auto match_int = match_when(s, integer);
    if (!match_int)    return none_token;

    auto match_frac = match_when(s, fractional);
    if (!match_frac)    return {TokenType::INTEGER, std::stoi(to_string(match_int.value()))};

    double val = std::stod(to_string(match_int.value()) + to_string(match_frac.value()));
    return {TokenType::DECIMAL, val};
}

// add number4 to your custom parser
```



This example takes the last one step further and generates a tokenizer given a matcher and tokenization algorithm. The tokenizer would then be passed a scan_view and return an optional token.

(Not yet implemented.)



```c++
matcher integer    = Match::at_least(1, is_digit);
matcher fractional = Match::join('.', integer);

number_token tokenize_int (string_view match)
{
    return {TokenType::INTEGER, std::stoi(to_string(match))};
};

number_token tokenize_dec (string_view match)
{
    return {TokenType::DECIMAL, std::stod(to_string(match))};
};

// Tokenize::incremental takes a list of pairs of scanners and functions
tokenizer number5 = Tokenize::incremental({integer,    tokenize_int},
                                          {fractional, tokenize_dec});

// add number5 to your custom parser
```



## DSL

Eventually a grammar will be added to the library as well, for traditional DSL creation. The main benefits of using a grammar is that scanning and parsing is handled for you.

(Not yet implemented.)



```c++
rule digit   = GrammarExp >> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
rule integer = *digit;
rule decimal = integer, '.', integer;

number_token tokenize_int (string_view match)
{
    return {TokenType::INTEGER, std::stoi(match)};
};

number_token tokenize_dec (string_view match)
{
    return {TokenType::DECIMAL, std::stod(match)};
};

Language myLang;
myLang.add_rule(integer, tokenize_int);
myLang.add_rule(decimal, tokenize_dec);
// Note, incremental parsing is handled automatically when rules are added
```





