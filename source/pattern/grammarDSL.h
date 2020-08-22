#ifndef GRAMMAR_DSL
#define GRAMMAR_DSL



// namespace GrammarExample
// {

// GrammarRule digit   = GrammarExpr << '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
// GrammarRule integer = +digit;
// GrammarRule decimal = integer + '.' + integer;

// number_token tokenize_int (string_view match)
// {
//     return {TokenType::INTEGER, std::stoi(match)};
// };

// number_token tokenize_dec (string_view match)
// {
//     return {TokenType::DECIMAL, std::stod(match)};
// };

// Language myLang;
// myLang.add_rule(integer, tokenize_int);
// myLang.add_rule(decimal, tokenize_dec);
// // Note, incremental parsing is handled automatically when rules are added

// } // namespace GrammarExample


namespace Pattern
{

/*
 * The Pattern Library grammar DSL provides a metasyntax for creating scanning and matching algorithms.
 * 
 * Being an embedded DSL, use of the notation must begin with a DSL-compatible type. A special object is provided to
 * mark the start of a new DSL expression in a convenient way. The following expression
 *     GrammarExpr << '0' | '1' | '2';
 * is equivalent to
 *     Pattern {'0'} | '1' | '2';
 * A grammar expression beginning with a pre-existing pattern may omit use of the special object.
 * 
 * The GrammarExpr object also receives configuration options through operator(), which will be passed to the
 * constructor of the new Pattern. For example, if your grammar treats space and newline, but not tabs, as whitespace,
 * you might write the following:
 *     auto my_lang_ws = [] (char c) { return c == ' ' || c == 'r' || c == 'n'; };
 *     GrammarExpr(my_lang_ws) << '0' | '1' | '2';
 * 
 * Since GrammarExpression.operator() returns a new GrammarExpression instance, you can bind it to create a new signal
 * based on your custom grammar. To bind the above whitespace definition, you might write the following:
 *     auto myGrammarExpr = GrammarExpr(my_lang_ws);
 * and then use it like so:
 *     auto pattern = myGrammarExpr << '0' | '1' | '2';
 * 
 * 
 * DSL syntax (m & n are integers):
 * GrammarExpr << a : Pattern {a}
 * 'a'              : Match::lit('a')
 * "a"              : Match::lit("a")
 * *a               : Match::many(a)
 * +a               : Match::at_least(1, a)
 * ~a               : Match::not(a)
 * --a              : Scan namespace version of rule "a", i.e. cuts its output from the parse tree
 * ++a              : Prevents wrapping the contents of rule "a" with Match::seq, i.e. splices the pattern into its surrounding context
 * a | b            : Match::any(a, b)
 * a & b            : Match::all(a, b)
 * a / b            : Match::all(a, not(b))
 * a >> b           : Match::seq(a, b)
 * {a, b}           : Match::seq(a, b)
 * a['b']           : Match::delim_seq(a, b)
 * a + b            : Match::join(a, b)
 * a * n            : Match::n_times(n, a)
 * a < n            : Match::at_most(n - 1, a)
 * a <= n           : Match::at_most(n, a)
 * a > n            : Match::at_least(n + 1, a)
 * a >= n           : Match::at_least(n, a)
 * m < a < n        : Match::rep(m + 1, n - 1, a)
 * (all converse operations with *, <, <=, >, & >= apply)
 * a(m, n)          : Match::rep(m, n, a)
 * 
 * In addition, all scanning and matching algorithms can be used wherever a rule or terminal is allowed.
 */

// Consider using EBNF forms [] for optional, {} for many, and () for sequence/grouping


template <typename... T>
class Pattern
{
public:

    Pattern ()
    {
        
    }

    bool create_output = true;
    std::tuple<T...> expressions;


}; // class Pattern



class GrammarExpression
{
public:
    template <typename T>
    Pattern operator<< (T t)
    {
        return {t};
    }

    template <typename Expr>
    GrammarExpression operator() (Expr is_whitespace)
    {
        // including other configuration options
    }

} GrammarExpr; // class GrammarExpression

} // namespace Pattern

#endif // GRAMMAR_DSL