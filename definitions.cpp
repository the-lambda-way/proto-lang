#include "definitions.h"


namespace Proto {

using namespace PatLib;
namespace PD = PatDef;

// ---------------------------------------------------------------------------------------------------------------------
// Recognizers
// ---------------------------------------------------------------------------------------------------------------------

Pattern whitespace = any({" ", "\r"});
Pattern comment    = PD::line_comment(lit("//"));
Pattern string     = PD::string();
Pattern integer    = PD::integer;





       default  :
            // Number
            if (isdigit(c)) {
                while (isdigit(scanner.peek()))    value += scanner.get();

                // Integer
                if (!scanner.peek() == '.') {
                    add_token(TokenType::INTEGER, value);
                    break;}

                // Decimal
                scanner.get();    // eat '.'

                if (!isdigit(scanner.peek())) {    // will be an error
                    scanner.unget();
                    break;}

                value += '.';
                while (isdigit(scanner.peek()))    value += scanner.get();
                add_token(TokenType::DECIMAL, value);
                break;

            
            // Identifier
            else if (isalpha(c)) {
                while (isalnum(scanner.peek()))    value += scanner.get();

                // Check if keyword
                auto it = keywords.find(value);
                if (it != keywords.end()) {
                    add_token(TokenType::KEYWORD, value);
                    break;}

                // Not a keyword
                add_token(TokenType::IDENTIFIER, value);
                break;}}}



// ---------------------------------------------------------------------------------------------------------------------
// Special actions
// ---------------------------------------------------------------------------------------------------------------------
void ignore_whitespace (string code)    return;


case '\n' : line_starts.push_back(scanner.tellg()); break;


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
    const void* value;};


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






}    // namespace Proto