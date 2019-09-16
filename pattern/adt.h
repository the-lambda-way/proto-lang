#include <variant>
#include <vector>

// Sum Types description: https://www.schoolofhaskell.com/school/to-infinity-and-beyond/pick-of-the-week/sum-types
// Detailed discussion of pattern matching in C++: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0095r1.html

namespace ADT {

template <class... Ts> struct PatternMatch : Ts... { using Ts::operator()...; };
template <class... Ts> PatternMatch(Ts...) -> PatternMatch<Ts...>;


template <typename... Members>
class SumType : public std::variant<Members...> {
using parent = std::variant<Members...>;

public:
    template <class T>
    constexpr SumType(T&& t) noexcept : parent(std::forward<T>(t)) {}

    template <class Visitor>
    void visit(Visitor v)    { std::visit(v, this); }
    
    template <typename... Callable>
    void match(Callable... c)    { std::visit(PatternMatch{c...}, this); }

    template <class Variant>
    SumType& operator= (Variant&& v) noexcept    { parent::operator=(std::forward<Variant>(v)); return *this; }

    template <typename T>
    T& get ()    { return std::get<T>(this); }
};


// Sugar construction
template <typename Out, typename... Left, typename... Right>
Out& operator| (SumType<Left...> left, const SumType<Right...>& right) {
    return SumType<Left..., Right...>();
}


// // SumType
// // ---------------------------------------------------------------------------------------------------------------------
// // Explicit construction
// template <typename... Members>
// struct SumType : public data, public std::variant<Members...> {};
// //using SumType = std::variant<Members...>;


// // Sugar construction
// template <typename Out, typename... Left, typename... Right>
// Out& operator| (SumType<Left...> left, const SumType<Right...>& right) {
//     return SumType<Left..., Right...>;
// }


// ProductType
// ---------------------------------------------------------------------------------------------------------------------
template <typename... Members>
class data : public std::tuple<Members...> {
using parent = std::tuple<Members...>;

public:
    template <typename... Ts>
    data (SumType<Ts...>&& s) : parent(std::forward<SumType<Ts...>>(s)) { }
};

struct unit {};
using Unit = SumType<unit>;


Unit False;
Unit True;
data Bool = {False | True};

;
// data False;
// data True;
// data Identifier;
// data Int;
// data Plus  = Literal("+");
// data Minus = Literal("-");
// data Star  = Literal("*");
// data Slash = Literal("/");
// data Equal = Literal("=");
// data Expression;

// data Bool = {False | True};
// data Equality = {Identifier, Equal, Identifier};
// data Equality = Identifier >> Equal >> Identifier;
// data Equality = Identifier ++ Equal ++ Identifier;
// data Addition = {Expression, Plus | Minus, Expression};

// data Calculate;
// data Arithmetic = {Calculate, (Expression, Plus | Minus, Expression) | (Expression, Star | Slash, Expression)};
// {data, SumType(SumType(data, SumType, data), SumType(data, SumType, data))}


// Bool isZero (Int i) {
    
// }


// String toBeOrNotToBe (Bool b) {}
// toBeOrNotToBe True  = "Be"
// toBeOrNotToBe False = "Not to be"

// main = putStrLn (toBeOrNotToBe True)
// main = print (True + 1)  -- Type error!



}    // namespace ADT


#include <string>
struct Quit {};
struct ChangeColor { int r, g, b; };
struct Move { int x; int y; };
struct Write { std::string message; };
SumType<Quit, ChangeColor, Move, Write> msg;

int main() {
    msg.match(
        [](const Quit&) { },
        [](const ChangeColor& cc) { },
        [](const Move& m) { },
        [](const Write& w) { }
    );
}