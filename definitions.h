#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <functional>    // macros
#include "pattern.h"     // reader
#include "syntax.h"      // token format


// ---------------------------------------------------------------------------------------------------------------------
// Recognizers
// ---------------------------------------------------------------------------------------------------------------------
// Single character


// Double character



// ---------------------------------------------------------------------------------------------------------------------
// Special actions
// ---------------------------------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------------------------------
// Tokenizers
// ---------------------------------------------------------------------------------------------------------------------

enum class TokenType;
enum class TokenLiteral;
struct Token;

Token tokenize_equal      (std::string code);
Token tokenize_colon      (std::string code);
Token tokenize_lparen     (std::string code);
Token tokenize_rparen     (std::string code);
Token tokenize_dot        (std::string code);
Token tokenize_lcurly     (std::string code);
Token tokenize_rcurly     (std::string code);
Token tokenize_comma      (std::string code);
Token tokenize_lbrace     (std::string code);
Token tokenize_rbrace     (std::string code);
Token tokenize_semicolon  (std::string code);
Token tokenize_underscore (std::string code);
Token tokenize_tab        (std::string code);




// ---------------------------------------------------------------------------------------------------------------------
// Expander
// ---------------------------------------------------------------------------------------------------------------------

// A Macro receives SourceCode and expands it, returning a new SourceCode object.
typedef std::function<Syntax::SyntaxObject (Syntax::SourceCode)> macro;


// A CodeProcessor manages syntax transformations over a body of code.
class CodeProcessor {
public:
    CodeProcessor ();

    template<typename Macro>
    void add (Macro&& macro)    { macros.push_back(std::forward<Macro>(macro)); }

    Syntax::SourceCode expand (Syntax::SourceCode code);

private:
    std::vector<macro> macros;
    Syntax::SourceCode code;
    int                index;
    int                indent;
};




#endif