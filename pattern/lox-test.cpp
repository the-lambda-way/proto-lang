// An implementation of the Lox language using the Pattern library
// http://www.craftinginterpreters.com/scanning.html
// http://www.craftinginterpreters.com/representing-code.html
// https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox

#ifndef LOXTEST
#define LOXTEST

#include <fstream>    // get_file_contents
#include <iostream>
#include <map>        // scanner keywords
#include <memory>     // unique_ptr
#include <string>
#include <vector>



enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END
};

// For importing TokenType members
namespace TokenTypeMembers {
    constexpr auto LEFT_PAREN    = TokenType::LEFT_PAREN;
    constexpr auto RIGHT_PAREN   = TokenType::RIGHT_PAREN;
    constexpr auto LEFT_BRACE    = TokenType::LEFT_BRACE;
    constexpr auto RIGHT_BRACE   = TokenType::RIGHT_BRACE;
    constexpr auto COMMA         = TokenType::COMMA;
    constexpr auto DOT           = TokenType::DOT;
    constexpr auto MINUS         = TokenType::MINUS;
    constexpr auto PLUS          = TokenType::PLUS;
    constexpr auto SEMICOLON     = TokenType::SEMICOLON;
    constexpr auto SLASH         = TokenType::SLASH;
    constexpr auto STAR          = TokenType::STAR;
    constexpr auto BANG          = TokenType::BANG;
    constexpr auto BANG_EQUAL    = TokenType::BANG_EQUAL;
    constexpr auto EQUAL         = TokenType::EQUAL;
    constexpr auto EQUAL_EQUAL   = TokenType::EQUAL_EQUAL;
    constexpr auto GREATER       = TokenType::GREATER;
    constexpr auto GREATER_EQUAL = TokenType::GREATER_EQUAL;
    constexpr auto LESS          = TokenType::LESS;
    constexpr auto LESS_EQUAL    = TokenType::LESS_EQUAL;
    constexpr auto IDENTIFIER    = TokenType::IDENTIFIER;
    constexpr auto STRING        = TokenType::STRING;
    constexpr auto NUMBER        = TokenType::NUMBER;
    constexpr auto AND           = TokenType::AND;
    constexpr auto CLASS         = TokenType::CLASS;
    constexpr auto ELSE          = TokenType::ELSE;
    constexpr auto FALSE         = TokenType::FALSE;
    constexpr auto FUN           = TokenType::FUN;
    constexpr auto FOR           = TokenType::FOR;
    constexpr auto IF            = TokenType::IF;
    constexpr auto NIL           = TokenType::NIL;
    constexpr auto OR            = TokenType::OR;
    constexpr auto PRINT         = TokenType::PRINT;
    constexpr auto RETURN        = TokenType::RETURN;
    constexpr auto SUPER         = TokenType::SUPER;
    constexpr auto THIS          = TokenType::THIS;
    constexpr auto TRUE          = TokenType::TRUE;
    constexpr auto VAR           = TokenType::VAR;
    constexpr auto WHILE         = TokenType::WHILE;
    constexpr auto END           = TokenType::END;
}

std::string to_string (TokenType type) {
    const std::string strings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
        "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",
        "IDENTIFIER", "STRING", "NUMBER",
        "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
        "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
        "END"
    };

    return strings[static_cast<int>(type)];
}

std::string to_string (std::string val)    { return val; }
std::string to_string (std::nullptr_t)     { return "nullptr"; }

template <typename T>
constexpr std::string to_string (T&& val)    { return std::to_string(std::forward<T>(val)); }


class TokenBase {
public:
    virtual std::string to_string () = 0;
};


template <typename ValueType>
struct Token : TokenBase {
    Token (TokenType type, std::string lexeme, ValueType literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line)
    {}

    std::string to_string () override {
        return ::to_string(type) + " " + lexeme + " " + ::to_string(literal);
    }

private:
    const TokenType   type;
    const std::string lexeme;
    const ValueType   literal;
    const int         line;
};


//const Interpreter interpreter();
bool had_error         = false;
bool had_runtime_error = false;


void report (int line, std::string where, std::string message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message;
    had_error = true;
}


void error (int line, std::string message) {
    report(line, "", message);
}


template <typename T>
void error (T token, std::string message) {
    if (token.type == TokenType::END)    report(token.line, " at end", message);
    else                                 report(token.line, " at '" + token.lexeme + "'", message);
    
}


// void runtime_error(RuntimeError error) {
//     std::cout << error.getMessage() << "\n[line " << error.token.line << "]";
//     had_runtime_error = true;
// }


class Scanner {
public:
    Scanner (std::string source);
    std::vector<std::unique_ptr<TokenBase>>& scan_tokens ();


private:
    const std::string       source;
    std::vector<std::unique_ptr<TokenBase>> tokens;

    int start   = 0;
    int current = 0;
    int line    = 1;

    const std::map<std::string, TokenType> keywords;

    void scan_token ();
    void identifier ();
    void number ();
    void string ();
    bool match (char expected);
    char peek ();
    char peek_next ();
    bool is_alpha (char c);
    bool is_alpha_numeric (char c);
    bool is_digit (char c);
    bool is_at_end ();
    char advance ();
    void add_token (TokenType type);

    template <typename ValueType>
    void add_token (TokenType type, ValueType literal);
};


Scanner::Scanner (std::string source) :
    source(source),
    keywords({
        {"and",    TokenType::AND},
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"false",  TokenType::FALSE},
        {"for",    TokenType::FOR},
        {"fun",    TokenType::FUN},
        {"if",     TokenType::IF},
        {"nil",    TokenType::NIL},
        {"or",     TokenType::OR},
        {"print",  TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super",  TokenType::SUPER},
        {"this",   TokenType::THIS},
        {"true",   TokenType::TRUE},
        {"var",    TokenType::VAR},
        {"while",  TokenType::WHILE},
    })
{}


std::vector<std::unique_ptr<TokenBase>>& Scanner::scan_tokens () {
    while (!is_at_end()) {
        // We are at the beginning of the next lexeme.
        start = current;
        scan_token();
    }

    tokens.emplace_back(new Token {TokenType::END, "", nullptr, line});
    return tokens;
}


void Scanner::scan_token () {
    char c = advance();

    switch (c) {
        using namespace TokenTypeMembers;

        case '(' : add_token(LEFT_PAREN);  break;
        case ')' : add_token(RIGHT_PAREN); break;
        case '{' : add_token(LEFT_BRACE);  break;
        case '}' : add_token(RIGHT_BRACE); break;
        case ',' : add_token(COMMA);       break;
        case '.' : add_token(DOT);         break;
        case '-' : add_token(MINUS);       break;
        case '+' : add_token(PLUS);        break;
        case ';' : add_token(SEMICOLON);   break;
        case '*' : add_token(STAR);        break;
        case '!' : add_token(match('=') ? BANG_EQUAL    : BANG);    break;
        case '=' : add_token(match('=') ? EQUAL_EQUAL   : EQUAL);   break;
        case '<' : add_token(match('=') ? LESS_EQUAL    : LESS);    break;
        case '>' : add_token(match('=') ? GREATER_EQUAL : GREATER); break;

        case '/':
            if (match('/'))
                // A comment goes until the end of the line.
                while (peek() != '\n' && !is_at_end())    advance();
            else
                add_token(SLASH);
            
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            ++line;
            break;
    
        case '"': string(); break;
    
        default:
                 if (is_digit(c))    number();
            else if (is_alpha(c))    identifier();
            else                     error(line, "Unexpected character.");
            break;
    };
}


void Scanner::identifier () {
    while (is_alpha_numeric(peek())) advance();

    // See if the identifier is a reserved word.
    std::string text = source.substr(start, current - start);

    auto match = keywords.find(text);
    
    if (match == keywords.end())    add_token(TokenType::IDENTIFIER, text);
    else                            add_token(match->second);
}


void Scanner::number () {
    while (is_digit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && is_digit(peek_next())) {
        // Consume the "."
        advance();

        while (is_digit(peek())) advance();
    }

    add_token(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}


void Scanner::string () {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n')    ++line;
        advance();
    }

    // Unterminated string.
    if (is_at_end()) {
        error(line, "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - 2 - start);
    add_token(TokenType::STRING, value);
}


bool Scanner::match (char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected)    return false;

    ++current;
    return true;
}


char Scanner::peek () {
    if (is_at_end()) return '\0';
    return source.at(current);
}


char Scanner::peek_next () {
    if (current + 1 >= source.length())    return '\0';
    return source.at(current + 1);
}


bool Scanner::is_alpha (char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           c == '_';
}


bool Scanner::is_alpha_numeric (char c) {
    return is_alpha(c) || is_digit(c);
}


bool Scanner::is_digit (char c) {
    return '0' <= c && c <= '9';
}
    

bool Scanner::is_at_end () {
    return current >= source.length();
}


char Scanner::advance () {
    ++current;
    return source.at(current - 1);
}


void Scanner::add_token (TokenType type) {
    add_token(type, nullptr);
}


template <typename ValueType>
void Scanner::add_token (TokenType type, ValueType literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(new Token {type, text, literal, line});
}


// ---------------------------------------------------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------------------------------------------------
void run (std::string source) {
    using namespace std;

    // Tokenize
    Scanner scanner(source);
    auto& tokens = scanner.scan_tokens();
    
    for (auto& token : tokens) {
        cout << token->to_string() << "\n";
    }

    // Parse
    // Parser parser(tokens);
    // std::vector<Stmt> statements = parser.parse();

    // Stop if there was a syntax error.
    // if (had_error)    return;


    // Resolve
    // Resolver resolver(interpreter);
    // resolver.resolve(statements);

    // Stop if there was a resolution error.
    // if (had_error)    return;


    // Interpret
    // interpreter.interpret(statements);
}


// source: http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents (std::string path) {
    using namespace std;

    // Open file
    ifstream file (path, ios::in | ios::binary | ios::ate);
    if (!file)    throw (errno);

    // Create string of sufficient size
    std::string contents;
    contents.resize(file.tellg());

    // Read complete file into string
    file.seekg(0, ios::beg);
    file.read(&contents[0], contents.size());

    return contents;
}


void run_file (std::string path) {
    std::string code = get_file_contents(path);
    run(code);

    if (had_error)            exit(EXIT_FAILURE);
    if (had_runtime_error)    exit(EXIT_FAILURE);
  }


void run_prompt () {
    using namespace std;

    string line;

    for (;;) {
        cout << "> ";
        getline(cin, line);
        run(line);

        had_error = false;
    }
}


int main (int argc, char* argv[]) {
    using namespace std;

    try {
             if (argc >  2)    cout << "Usage: lox [script]";
        else if (argc == 2)    run_file(argv[0]);
        else                   run_prompt();
    }
    catch (exception& e) {
        cerr << e.what();
    }
}


/*
// Grammar
program        → declaration* EOF ;
declaration    → classDecl
               | funDecl
               | varDecl
               | statement ;
classDecl      → "class" IDENTIFIER ( "<" IDENTIFIER )?
                 "{" function* "}" ;
funDecl        → "fun" function ;
varDecl        → "var" IDENTIFIER ( "=" expression )? ";" ;
statement      → exprStmt
               | forStmt
               | ifStmt
               | printStmt
               | returnStmt
               | whileStmt
               | block ;
exprStmt       → expression ";" ;
forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
                           expression? ";"
                           expression? ")" statement ;
ifStmt         → "if" "(" expression ")" statement ( "else" statement )? ;
printStmt      → "print" expression ";" ;
returnStmt     → "return" expression? ";" ;
whileStmt      → "while" "(" expression ")" statement ;
block          → "{" declaration* "}" ;
expression     → assignment ;
assignment     → ( call "." )? IDENTIFIER "=" assignment
               | logic_or;

logic_or       → logic_and ( "or" logic_and )* ;
logic_and      → equality ( "and" equality )* ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
addition       → multiplication ( ( "-" | "+" ) multiplication )* ;
multiplication → unary ( ( "/" | "*" ) unary )* ;

unary          → ( "!" | "-" ) unary | call ;
call           → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
primary        → "true" | "false" | "nil" | "this"
               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
               | "super" "." IDENTIFIER ;
function       → IDENTIFIER "(" parameters? ")" block ;
parameters     → IDENTIFIER ( "," IDENTIFIER )* ;
arguments      → expression ( "," expression )* ;
NUMBER         → DIGIT+ ( "." DIGIT+ )? ;
STRING         → '"' <any char except '"'>* '"' ;
IDENTIFIER     → ALPHA ( ALPHA | DIGIT )* ;
ALPHA          → 'a' ... 'z' | 'A' ... 'Z' | '_' ;
DIGIT          → '0' ... '9' ;
*/


#endif    // LOXTEST