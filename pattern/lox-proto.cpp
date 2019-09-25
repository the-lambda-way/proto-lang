// An implementation of the Lox language using the Pattern library
// http://www.craftinginterpreters.com/scanning.html
// http://www.craftinginterpreters.com/representing-code.html
// https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox


#ifndef LOXPROTOTEST
#define LOXPROTOTEST

#include "pattern2.cpp"

#include <fstream>     // get_file_contents
#include <iostream>    // getline
#include <map>         // keywords
#include <memory>      // unique_ptr
#include <string>
#include <vector>

using namespace PL;
using std::string_view;


// ---------------------------------------------------------------------------------------------------------------------
// Test
// ---------------------------------------------------------------------------------------------------------------------
namespace LoxLang
{

// ---------------------------------------------------------------------------------------------------------------------
//  Definitions
// ---------------------------------------------------------------------------------------------------------------------
constexpr bool is_digit (char c)            { return '0' <= c && c <= '9'; }
constexpr bool is_alpha (char c)            { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'; }
constexpr bool is_alpha_numeric (char c)    { return is_alpha(c) || is_digit(c); }
constexpr bool is_whitespace (char c)       { return c == ' ' || c == '\r' || c == '\t'; }

using namespace Scan;

scanner alpha      = when(is_alpha);
scanner alpha_nums = while_it(is_alpha_numeric);
scanner identifier = join(alpha, optional(alpha_nums));
scanner digits     = while_it(is_digit);
scanner number     = join(digits, optional(join(lit('.'), digits)));
scanner string     = join(lit('"'), until('"'));
scanner whitespace = when(is_whitespace);


enum class TokenType
{
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
namespace TokenTypeMembers
{
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

template <typename T>
std::string to_string (T val)              { return std::to_string(val); }
std::string to_string (string_view s)      { return PL::to_string(s); }
std::string to_string (std::nullptr_t)     { return "nullptr"; }


struct TokenBase {
    virtual std::string to_string () = 0;
};


template <typename ValueType>
struct Token : TokenBase {
    Token (TokenType type, string_view lexeme, ValueType literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line)
    {}

    std::string to_string () override {
        return LoxLang::to_string(type) + " " + lexeme + " " + LoxLang::to_string(literal);
    }

    const TokenType   type;
    const std::string lexeme;
    const ValueType   literal;
    const int         line;
};


const std::map<std::string, TokenType> keywords =
    {
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
    };


// ---------------------------------------------------------------------------------------------------------------------
//  Engine
// ---------------------------------------------------------------------------------------------------------------------
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


class Scanner
{
public:
    Scanner (const std::string source);
    std::vector<std::unique_ptr<TokenBase>>& scan_tokens ();

private:
    // Traversal
    const std::string source;
    string_view view;
    int line = 1;

    char peek ()                         { return ::peek(view); }
    char peek_next ()                    { return ::peek2(view); }
    void advance ()                      { ::advance(view); }
    bool is_at_end ()                    { return view.empty(); }
    bool match (char expected)           { return scan_when(view, expected); }
    bool match (std::string expected)    { return scan_when(view, expected); }

    // Tokenizers
    std::vector<std::unique_ptr<TokenBase>> tokens;
    const std::map<std::string, TokenType> keywords;

    void identifier ();
    void number ();
    void string ();

    // Execute
    void scan_token ();
    void add_token (TokenType type, string_view lexeme);

    template <typename ValueType>
    void add_token (TokenType type, string_view lexeme, ValueType literal);
};


Scanner::Scanner (std::string source) :
    source(source),
    view(this->source),
    keywords(LoxLang::keywords)
{}


void Scanner::identifier ()
{
    string_view match = match_when(view, LoxLang::identifier).value();

    auto kw_match = keywords.find(to_string(match));
    if (kw_match == keywords.end())    add_token(TokenType::IDENTIFIER, match, match);
    else                               add_token(kw_match->second, match);
}


void Scanner::string ()
{
    string_view match = match_when(view, LoxLang::string).value();
                                   
    add_token(TokenType::STRING, "", match.substr(1, match.length() - 2));
}


void Scanner::number ()
{
    string_view match = match_when(view, LoxLang::number).value();

    add_token(TokenType::NUMBER, match, std::stod(to_string(match)));
}


void Scanner::scan_token ()
{
    bool found_simple = true;    // helps avoid backtracking
    char c = peek();

    switch (c)
    {
        using namespace TokenTypeMembers;

        case '(' : add_token(LEFT_PAREN, "(");  break;
        case ')' : add_token(RIGHT_PAREN, ")"); break;
        case '{' : add_token(LEFT_BRACE, "[");  break;
        case '}' : add_token(RIGHT_BRACE, "]"); break;
        case ',' : add_token(COMMA, ",");       break;
        case '.' : add_token(DOT, ".");         break;
        case '-' : add_token(MINUS, "-");       break;
        case '+' : add_token(PLUS, "+");        break;
        case ';' : add_token(SEMICOLON, ";");   break;
        case '*' : add_token(STAR, "*");        break;
        case '!' : peek_next() == '=' ? add_token(BANG_EQUAL, "!=")    : add_token(BANG, "!");    break;
        case '=' : peek_next() == '=' ? add_token(EQUAL_EQUAL, "==")   : add_token(EQUAL, "=");   break;
        case '<' : peek_next() == '=' ? add_token(LESS_EQUAL, "<=")    : add_token(LESS, "-");    break;
        case '>' : peek_next() == '=' ? add_token(GREATER_EQUAL, ">=") : add_token(GREATER, ">"); break;

        case '/':
            if   (peek_next() != '/')    add_token(SLASH, "/");
            else                         found_simple = false;    // is a comment
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            ++line;
            break;
    
        default:
            found_simple = false;
    }

    if (found_simple)    advance();
    else
    {
             if (c == '"')       string();
        else if (is_digit(c))    number();
        else if (is_alpha(c))    identifier();
        else if (match("//"))    scan_while_not(view, '\n');    // is a comment
        else                     error(line, "Unexpected character.\n");
    }
}


void Scanner::add_token (TokenType type, string_view lexeme)
{
    add_token(type, lexeme, nullptr);
}


template <typename ValueType>
void Scanner::add_token (TokenType type, string_view lexeme, ValueType literal)
{
    tokens.emplace_back(new Token {type, lexeme, literal, line});
}


std::vector<std::unique_ptr<TokenBase>>& Scanner::scan_tokens () {
    while (!is_at_end())    scan_token();

    add_token(TokenType::END, "\\0", nullptr);
    return tokens;
}

} // namespace LoxLang



// ---------------------------------------------------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------------------------------------------------
void run (std::string source) {
    using namespace std;
    using namespace LoxLang;

    // Tokenize
    Scanner scanner(source);
    auto& tokens = scanner.scan_tokens();
    
    for (auto& token : tokens) {
        cout << token->to_string() << "\n";
    }
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
    using namespace LoxLang;

    std::string code = get_file_contents(path);
    run(code);

    if (had_error)            exit(EXIT_FAILURE);
    if (had_runtime_error)    exit(EXIT_FAILURE);
  }


void run_prompt () {
    using namespace std;
    using namespace LoxLang;

    std::string line;

    for ( ; !cin.eof(); ) {
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
        else if (argc == 2)    run_file(argv[1]);
        else                   run_prompt();
    }
    catch (exception& e) {
        cerr << e.what();
    }
}


#endif    // LOXPROTOTEST