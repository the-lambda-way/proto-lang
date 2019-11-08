// The pattern library supports an evolution of scanning abstractions, from low-level procedural code to high-level
//     declarative code.

#include <string>
#include <variant>
#include "../include/scanning-algorithms.h"
#include "../include/scan_view.h"
#include "../include/syntax.h"



// -------------------------
// Number
// -------------------------
// The following example shows how to scan and tokenize a number, which is defined as either an integer or a decimal. An
// integer here is defined as a sequence of digits, and a decimal as an integer, followed by '.', followed by an
// integer. We'll also showcase incrementally tokenizing both integers and decimals in the same function.

enum class TokenType    { INTEGER, DECIMAL, NONE };
using number_token = token<TokenType, std::variant<std::monostate, int, double>>;

number_token none_token {TokenType::NONE, std::monostate {}};
constexpr bool is_digit (char c)    { return '0' <= c && c <= '9'; }

std::string to_string (std::string_view s)    { return std::string {s.data(), s.length()}; }


// -------------------------
// Pointer iteration
// -------------------------
namespace PointerExample
{

number_token number (scan_view& s)
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

} // namespace PointerExample


// -------------------------
// Algorithms
// -------------------------
namespace AlgorithmExample
{

number_token number2 (scan_view& s)
{
    s.save();

    // Integer
    if (!advance_if(s, is_digit))    return none_token;
    advance_while(s, is_digit);

    if (*s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};

    // Decimal
    s += 2;
    advance_while(s, is_digit);

    return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}

// add number2 to your custom parser

} // namespace AlgorithmExample


// -------------------------
// Algorithm syntactic sugar
// -------------------------
namespace AlgorithmSugarExample
{

number_token number3 (scan_view& s)
{
    s.save();

    // Integer
    if (!(s >> is_digit))    return none_token;
    s >> *is_digit;

    if (!(s >> '.' & is_digit))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};

    // Decimal
    s >> *is_digit;

    return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}

// add number3 to your custom parser

} // namespace AlgorithmSugarExample


// -------------------------
// Higher-order functions
// -------------------------
namespace HigherOrderExample
{

scanner integer    = Scan::at_least(1, is_digit);
scanner fractional = Scan::join('.', integer);

number_token number4 (scan_view& s)
{
    if (!integer(s))       return none_token;
    if (!fractional(s))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};
    return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
}

// add number4 to your custom parser

} // namespace HigherOrderExample


// -------------------------
// Declarative
// -------------------------
namespace DeclarativeExample
{

scanner integer    = Scan::at_least(1, is_digit);
scanner fractional = Scan::join('.', integer);

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

} // namespace DeclarativeExample


// -------------------------
// Grammar
// -------------------------
namespace GrammarExample
{

rule digit   = GrammarExp >> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
rule integer = +digit;
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

} // namespace GrammarExample


// -------------------------
// Extendible auto-language using CRTP magic
// -------------------------
namespace CRTPExample
{

Parser myLang;
scanner integer    = Scan::at_least(1, is_digit);
scanner fractional = Scan::join('.', integer);

struct number6 : AddRule<MyLang, number6>
{
    parse_tree parse (scan_view& s)    { return match_impl(s); }

    token action (parse_tree match)
    {
        auto [a, b] = match;    // a, b are string_views

        if (b == "")    return {TokenType::INTEGER, std::stoi(to_string(a))};

        double val = std::stod(to_string(a) + "." + to_string(b));
        return {TokenType::DECIMAL, val};
    }

private:
    matcher match_impl = Match::seq(integer, Scan::opt(fractional));
}

// myLang.parse(string_view source) automatically calls all rules added with this method

} // namespace CRTPExample
