.. comment
    For an automated documentation build solution, see
    https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

########################################################################################################################
Proto Language
########################################################################################################################

.. toctree::
    :maxdepth: 2
    :caption: Contents:

    pattern



########################################################################################################################
Indices and tables
########################################################################################################################

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`



########################################################################################################################
Pattern Library Quick Look
########################################################################################################################


The Pattern Library is a toolkit for building recursive descent parsers, offering a range of features from low-level pointer manipulation, to high-level, grammar-like abstractions.

Below are various ways in which you might tokenize a number which can be either an integer or a decimal. Assume that the current character has been identified as a digit in your outer loop.


========================================================================================================================
Set up
========================================================================================================================
The following definitions are used throughout the examples:

.. code-block::

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


========================================================================================================================
Using pointers
========================================================================================================================

.. code-block::

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


========================================================================================================================
Using algorithms
========================================================================================================================

.. code-block::

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


========================================================================================================================
Using higher-order functions
========================================================================================================================

.. code-block::

     number_token number (scan_view& s)
     {
          constexpr auto integer    = Scan::some(is_digit);
          constexpr auto fractional = Scan::join('.', integer);

          s.save();

          integer(s);

          if (!fractional(s))     return {TokenType::INTEGER, std::stoi(s.copy_skipped())};
          return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
     }


========================================================================================================================
Using declarative features
========================================================================================================================

.. code-block::

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


========================================================================================================================
Using grammar-like features
========================================================================================================================

.. code-block::

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


========================================================================================================================
Using built-in definitions
========================================================================================================================

.. code-block::

     #include "pattern.h"

     Language myLang;
     myLang.add_rule(PatDef::integer, [](auto m) -> number_token { return {TokenType::INTEGER, std::stoi(m)}; });
     myLang.add_rule(PatDef::decimal, [](auto m) -> number_token { return {TokenType::DECIMAL, std::stod(m)}; });

