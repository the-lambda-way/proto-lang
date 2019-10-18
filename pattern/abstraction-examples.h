// The pattern library supports an evolution of scanning abstractions, from low-level procedural code to high-level
//     declarative code.

#include <string>
#include <variant>
#include "../include/scanning-algorithms.h"
#include "../include/scanner.h"



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

} // namespace PointerExample


// -------------------------
// Algorithms
// -------------------------
namespace AlgorithmExample
{

number_token number2 (scanner& s)
{
    s.save();

    // Integer
    if (!advance_if(s, is_digit))    return none_token;
    advance_while(s, is_digit);

    if (s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.to_string())};

    // Decimal
    s += 2;
    advance_while(s, is_digit);

    return {TokenType::DECIMAL, std::stod(s.to_string())};
}

// add number2 to your custom parser

} // namespace AlgorithmExample


// -------------------------
// Algorithm syntactic sugar
// -------------------------
namespace AlgorithmSugarExample
{

// To use unary operators (like Kleene star *), a function must be converted to a scanning_expression
scanning_expression digit {is_digit};

number_token number3 (scanner& s)
{
    s.save();

    // Integer
    if (!(s >> digit))    return none_token;
    s >> *digit;

    if (s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.to_string())};

    // Decimal
    s += 2;
    s >> *digit;

    return {TokenType::DECIMAL, std::stod(s.to_string())};
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

number_token number4 (scanner& s)
{
    auto match_int = match_when(s, integer);
    if (!match_int)    return none_token;

    auto match_frac = match_when(s, fractional);
    if (!match_frac)    return {TokenType::INTEGER, std::stoi(match_int.value())};

    double val = std::stod(to_string(match_int.value()) + to_string(match_frac.value()));
    return {TokenType::DECIMAL, val};
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
    return {TokenType::INTEGER, std::stoi(match)};
};

number_token tokenize_dec (string_view match)
{
    return {TokenType::DECIMAL, std::stod(match)};
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
    parse_tree parse (scanner& s)    { return match_impl(s); }

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
