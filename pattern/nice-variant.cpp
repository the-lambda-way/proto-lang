#include <utility>
#include <variant>


// // Bring all callables into scope
template <class... Ts> struct NiceVisitor : Ts... { using Ts::operator()...; };
template <class... Ts> NiceVisitor(Ts...) -> NiceVisitor<Ts...>;


// From https://youtu.be/CELWr9roNno?t=1146
template <typename Variant, typename... Handlers>
auto match (Variant&& v, Handlers&&... handlers) {
    return std::visit(NiceVisitor { std::forward<Handlers>(handlers)... },
                      std::forward<Variant>(v)
    );
}


// template <typename... Ts>
// class NiceVariant : public std::variant<Ts...> {
// using parent = std::variant<Ts...>;

// public:
//     template <class T>
//     constexpr NiceVariant(T&& t) noexcept : parent(std::forward<T>(t)) {}

//     template <class NiceVisitor>
//     void visit(NiceVisitor v)     { std::visit(v, this); }
    
//     template <typename... callables>
//     void match(callables... c)    { std::visit(NiceVisitor{c...}, this); }

//     template <class T>
//     NiceVariant& operator= (T&& t) noexcept    { parent::operator=(std::forward<T>(t)); return *this; }  
// };

template <typename Derived, typename... DerivedMembers>
class _NiceVariant : std::variant<DerivedMembers...> {
public:

    // Note: the compiler does not perform non-dependent name lookup on dependent base classes
    using container = std::variant<DerivedMembers...>;
    using container::variant;

    template <typename Visitor>
    void visit (Visitor& v)    { impl->visit(v, impl); }

private:
    Derived* impl = static_cast<Derived*>(this);
};


// https://en.wikipedia.org/wiki/Template_metaprogramming#Static_polymorphism
// Order of instantiation: derived memory, base layout, derived layout
template <typename... Members>
class NiceVariant : private _NiceVariant<NiceVariant<Members...>, Members...> {

public:
    template <typename Visitor>
    void visit (Visitor& v)    { std::visit(v, this); }


};


// // Example usage
// // ---------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <variant>

// Example 1
// https://www.walletfox.com/course/patternmatchingcpp17.php
#include <cmath>
using var_roots = NiceVariant<std::pair<double,double>, double, std::monostate>;

var_roots computeRoots(double a, double b, double c){
    auto d = b*b-4*a*c; // discriminant
    if (d > 0.0) {
        auto p = sqrt(d) / (2*a);
        return std::make_pair(-b + p, -b - p);
    }
    else if (d == 0.0)
        return (-1*b)/(2*a);
    else
        return std::monostate();
}

NiceVisitor printQuadResult = {
    [](const std::pair<double,double>& arg) { std::cout << "2 roots found: " << arg.first << " " << arg.second << '\n'; },
    [](double arg)                          { std::cout << "1 root found: " << arg << '\n'; }, 
    [](std::monostate)                      { std::cout << "No real roots found.\n"; },
};


// Example 2
// https://chadaustin.me/2015/07/sum-types/
struct Quit        {};
struct ChangeColor { int r, g, b; };
struct Move        { int x; int y; };
struct Write       { std::string message; };
NiceVariant<Quit, ChangeColor, Move, Write> msg;


int main() {
    // Example 1
    match(computeRoots(1,0,-1),  printQuadResult);  // 2 roots found: 1 -1
    match(computeRoots(1,-2,-2), printQuadResult);  // 2 roots found: 3.73205 0.267949
    match(computeRoots(1,6,9),   printQuadResult);  // 1 root found: -3
    match(computeRoots(1,-3,4),  printQuadResult);  // No real roots found.

    computeRoots(1,0,-1).visit(printQuadResult);  // 2 roots found: 1 -1
    computeRoots(1,-2,-2).visit(printQuadResult); // 2 roots found: 3.73205 0.267949
    computeRoots(1,6,9).visit(printQuadResult);   // 1 root found: -3
    computeRoots(1,-3,4).visit(printQuadResult);  // No real roots found.


    // Example 2
    msg = "Hi!";
    msg.match(
        [](const Quit&) { },
        [](const ChangeColor& cc) { },
        [](const Move& m) { },
        [](const Write& w) { }
    );
}



