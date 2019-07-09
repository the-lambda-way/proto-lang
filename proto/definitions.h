#ifndef DEFINITIONS_H
#define DEFINITIONS_H


// Some Parts
// tags
// modules, imports, exports
// blocks
// frames
// if then else
// identifier binding, rebinding
// datum, syntax object
// goto
// procedures, procedure calls
// syntax transformers
// parameter sets
// compilation phases
// general compile-time evaluation
// arrays
// structures
// encodings
// Systems


// WASM Structure

// Frame
// functions, tables, memories, globals

// Encodings
// int, float

// Module
// types, function pointers, table pointers, memories pointers, global pointers, exports, start function, initialization

// Function
// type, module (closes over), body

// Table
// function pointers

// Memories
// raw bytes

// Globals
// value, mutability

// Export
// name, value (function | table | memory | global)

// Stack
// instructions | goto labels | frames

// Traps
// propogate up through the call stack


// ---------------------------------------------------------------------------------------------------------------------

// ID (primitive)
// A globally unique identifier
// Possible implementations: a memory address; a frame member pointer

// Identifier (primitive)
// A transitory representation of a GUID
// Possible implementations: a textual symbol

// Symbol (primitive)
// A textual unit

// Record
// An unordered collection of name-value pairs

// ---------------------------------------------------------------------------------------------------------------------


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

Token tokenize_equal      (string code);
Token tokenize_colon      (string code);
Token tokenize_lparen     (string code);
Token tokenize_rparen     (string code);
Token tokenize_dot        (string code);
Token tokenize_lcurly     (string code);
Token tokenize_rcurly     (string code);
Token tokenize_comma      (string code);
Token tokenize_lbrace     (string code);
Token tokenize_rbrace     (string code);
Token tokenize_semicolon  (string code);
Token tokenize_underscore (string code);
Token tokenize_tab        (string code);









#endif