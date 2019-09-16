#include "pattern2.cpp"
#include <string>
#include <string_view>
#include <tuple>           // std::ignore
using std::string_view;

namespace PD {
    auto _ = std::ignore;
}



Pattern group (Pattern pattern)    { return seq {"(", pattern, ")"}; }

Pattern identifier = seq {letter, rep(alphanum)};
Pattern type       = any {"int", "char"};
Pattern value      = any {identifier, at_least(1, any {integer, decimal})};

Pattern parameter = seq {type, value};
Pattern parameter_pack ()                 { return parameter_pack(0, -1); };
Pattern parameter_pack (int n)            { return parameter_pack(n, n); ;
Pattern parameter_pack (int min, int max) {
    if constexpr (min > 0)
        return group( seq {parameter, rep(seq {",", parameter}, min - 1, max - 1)});
    else
        return group( optional(seq {parameter, rep(seq {",", parameter}, min - 1, max - 1)}));

};

Pattern func_decl  = seq {"function", identifier, parameter_pack};
Pattern expression = seq {any {func_decl, value}, ";"};


// // Perhaps you want a custom language construct.
// Pattern add3_signature = seq {"add3", parameter_pack(3)};

// std::string add3_macro (string_view parse_tree) {
//     auto [_, _, _, t1, v1, t2, v2, t3, v3, _, _] = flatten(parse_tree);
//     return std::to_string(std::stoi(v1) + std::stoi(v2) + std::stoi(v3));
// }

// parser.add_macro(add3_signature, add3_macro);


// // A more declarative option. macro<macro_type> adds macro_type to the parser when its constructor is called.
// struct add2 : macro<add2> {
//     ParseTree match (string_view& source)    { return seq {"add2", parameter_pack(2)}; }

//     std::string operator() (string_view parse_tree) {
//         auto [_, _, _, t1, v1, t2, v2, _, _] = flatten(parse_tree);
//         return std::to_string(std::stoi(v1) + std::stoi(v2));
//     }
// };


int main (int, char**) {
    using namespace PD;    // PD::_

    std::string some_code = "function add3 (int a, int b, int c);";
    // {{"function",
    //   "add3",
    //       {"(",
    //           {{"int", "1"},
    //            {"int", "2"},
    //            {"int", "3"}},
    //        ")"}},
    //  ";"};
    
    auto [func, _]       = expression(some_code);
    auto [_, id, params] = func;
    auto [p1, p2, p3]    = params;
    auto [t1, v1]        = p1;
    auto [t2, v2]        = p2;
    auto [t3, v3]        = p3;
    
    std::cout << id << " " << t1 << " " << v1 << " " << t2 << " " << v2 << " " << t3 << " " << v3 << "\n";

    // Or, flatten it first
    auto [_, id, _, t1, v1, t2, v2, t3, v3, _, _] = flatten(expression(some_code));
    std::cout << id << " " << t1 << " " << v1 << " " << t2 << " " << v2 << " " << t3 << " " << v3 << "\n";
}

