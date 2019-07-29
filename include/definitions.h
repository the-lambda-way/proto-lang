#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <functional>    // macros
#include "pattern.h"     // reader
#include "syntax.h"      // token format

using namespace PL;
using namespace Syntax;


// ---------------------------------------------------------------------------------------------------------------------
// Recognizers
// ---------------------------------------------------------------------------------------------------------------------
Pattern whitespace = any({" ", "\r"});
Pattern comment    = PD::line_comment(lit("//"));
Pattern string     = PD::string();
Pattern integer    = PD::integer;
Pattern decimal    = PD::decimal;
Pattern identifier = seq({PD::letter, rep(PD::alphanum)});
Pattern bracket    = any({"(", ")", "{", "}", "[", "]"});
Pattern bin_op     = any({"=", ":", ".", ",", ";", "<", ">", "+", "-", "*", "/",
                          "!=", "<=", ">=", "++"});
Pattern unary_op   = any({"!", "'"});
Pattern tab        = lit("\t");
Pattern boolean    = any({"true", "false"});
Pattern literal    = any({integer, decimal, ::string, boolean, lit("nil")});
Pattern grouping   = seq({lit("("), expression, lit(")")});
Pattern unary_proc = seq({unary_op, expression});
Pattern bin_proc   = seq({expression, bin_op, expression});
Pattern infix_proc = seq({expression, PD::at_least(1, seq({bin_op, expression}))});
Pattern expression = any({unary_proc, bin_proc, infix_proc, grouping, literal});


// ---------------------------------------------------------------------------------------------------------------------
// Tokenizers
// ---------------------------------------------------------------------------------------------------------------------
enum class TokenType {
    LITERAL, IDENTIFIER, INTEGER, DECIMAL, STRING, KEYWORD, END_OF_FILE};

enum class TokenLiteral {
    // Double-character
    QUOTE_OPEN, QUOTE_CLOSE, NOT_EQUAL, QUASI_OPEN, QUASI_CLOSE, PLUSPLUS, STARSTAR, UNQUOTE, LT_EQUAL, GT_EQUAL,

    // Triple-character
    QUASISYNTAX_OPEN, PLUSPLUSEQUAL};


struct Token {
    const int   type;
    const void* value;
};




// Single character
Token tokenize_equal        (string code)    return {TokenType::LITERAL, '='};
Token tokenize_colon        (string code)    return {TokenType::LITERAL, ':'};
Token tokenize_lparen       (string code)    return {TokenType::LITERAL, '('};
Token tokenize_rparen       (string code)    return {TokenType::LITERAL, ')'};
Token tokenize_dot          (string code)    return {TokenType::LITERAL, '.'};
Token tokenize_lcurly       (string code)    return {TokenType::LITERAL, '{'};
Token tokenize_rcurly       (string code)    return {TokenType::LITERAL, '}'};
Token tokenize_comma        (string code)    return {TokenType::LITERAL, ','};
Token tokenize_lbrace       (string code)    return {TokenType::LITERAL, '['};
Token tokenize_rbrace       (string code)    return {TokenType::LITERAL, ']'};
Token tokenize_semicolon    (string code)    return {TokenType::LITERAL, ';'};
Token tokenize_underscore   (string code)    return {TokenType::LITERAL, '_'};
Token tokenize_tab          (string code)    return {TokenType::LITERAL, '\t'};
Token tokenize_bang         (string code)    return {TokenType::LITERAL, '!'};
Token tokenize_less_than    (string code)    return {TokenType::LITERAL, '<'};
Token tokenize_greater_than (string code)    return {TokenType::LITERAL, '>'};
Token tokenize_quote        (string code)    return {TokenType::LITERAL, "'"};
Token tokenize_plus         (string code)    return {TokenType::LITERAL, '+'};
Token tokenize_slash        (string code)    return {TokenType::LITERAL, '/'};

// Double character
Token tokenize_not_equal   (string code)    return {TokenType::LITERAL, TokenLiteral::NOT_EQUAL};
Token tokenize_lt_equal    (string code)    return {TokenType::LITERAL, TokenLiteral::LT_EQUAL};
Token tokenize_gt_equal    (string code)    return {TokenType::LITERAL, TokenLiteral::GT_EQUAL};
Token tokenize_quote_open  (string code)    return {TokenType::LITERAL, TokenLiteral::QUOTE_OPEN};
Token tokenize_quote_close (string code)    return {TokenType::LITERAL, TokenLiteral::QUOTE_CLOSE};
Token tokenize_quasi_close (string code)    return {TokenType::LITERAL, TokenLiteral::QUASI_CLOSE};
Token tokenize_starstar    (string code)    return {TokenType::LITERAL, TokenLiteral::STARSTAR};
Token tokenize_quasi_open  (string code)    return {TokenType::LITERAL, TokenLiteral::QUASI_OPEN};
Token tokenize_unquote     (string code)    return {TokenType::LITERAL, TokenLiteral::UNQUOTE};
Token tokenize_plusplus    (string code)    return {TokenType::LITERAL, TokenLiteral::PLUSPLUS};

// Triple character
Token tokenize_quasisyntax_open (string code)    return {TokenType::LITERAL, TokenLiteral::QUASISYNTAX_OPEN};
Token tokenize_plusplus_equal   (string code)    return {TokenType::LITERAL, TokenLiteral::PLUSPLUSEQUAL};

// Special
Token tokenize_string     (string code)    return {TokenType::STRING, code.substr(1, code.size() - 1)};
Token tokenize_integer    (string code)    return {TokenType::INTEGER, code};
Token tokenize_decimal    (string code)    return {TokenType::DECIMAL, code};
Token tokenize_identifier (string code)    return {Tokentype::IDENTIFIER, code};
Token tokenize_keyword    (string code)    return {Tokentype::KEYWORD, code};



 
// Unknown character error
int line, column;
get_location(last_pos, line, column);

System::fail("Unexpected character",
            get_line_code(line),
            origin,
            line,
            column);


// String error, put into code mangager?
if (scanner.eof()) {
    int line, column;
    get_location(last_pos, line, column);

    System::fail("Unterminated string.",
                    get_line_code(line),
                    origin,
                    line,
                    column);




// ---------------------------------------------------------------------------------------------------------------------
// Expander
// ---------------------------------------------------------------------------------------------------------------------

// A Macro receives SourceCode and expands it, returning a new SourceCode object.
typedef std::function<SyntaxObject (SourceCode)> macro;


// A CodeProcessor manages syntax transformations over a body of code.
class CodeProcessor {
public:
    CodeProcessor ();

    template<typename Macro>
    void add (Macro&& macro)    { macros.push_back(std::forward<Macro>(macro)); }

    SourceCode expand (SourceCode code);

private:
    std::vector<macro> macros;
    SourceCode         code;
    int                index;
    int                indent;
};




#endif