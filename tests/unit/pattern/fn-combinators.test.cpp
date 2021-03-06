#include <functional>     // std::ref, std::reference_wrapper
#include <tuple>
#include <utility>        // std::forward

#include "catch2/catch.hpp"
#include "pattern/fn-combinators.h"


using namespace Pattern;


// =====================================================================================================================
// fn::bind_back
// =====================================================================================================================
SCENARIO("The bind_back function should return an object.")
{
     auto an_object = fn::bind_back([] () { return true; });
}


SCENARIO("The bind_back function should take a bound function by any value category.")
{
     GIVEN("an lvalue, an xvalue, and a prvalue of a function")
     {
          // Reference: https://en.cppreference.com/w/cpp/language/decltype

          auto returns_true = [] () { return true; };


          THEN("the bind_back function can be called with each one.")
          {
               // Cannot construct an assertion with an overloaded function. But if it compiles, it passed.
               fn::bind_back(returns_true);
               fn::bind_back(std::move(returns_true));
               fn::bind_back([] () { return true; });
          }
     }
}


SCENARIO("The bind_back function should be invocable with any callable type.")
{
     GIVEN("Every type of callable object")
     {
          // Although instances of every type are not explictly needed for the tests, it helps with comprehension.
          bool (*fun_ptr) () = [] () { return true; };
          auto closure = [] () { return true; };

          struct a_class
          {
               bool fun () { return true; }

               // Data members are callable, see https://en.cppreference.com/w/cpp/named_req/Callable
               bool data;
          };

          auto ptr_to_mbr_function = &a_class::fun;
          auto ptr_to_data_mbr = &a_class::data;

          struct fun_object_t
          {
               bool operator() () { return true; }
          } fun_object;


          THEN("the bind_back function can be called with each object.")
          {
               // Cannot construct an assertion with an overloaded function. But if it compiles, it passed.
               fn::bind_back(fun_ptr);
               fn::bind_back(closure);
               fn::bind_back(ptr_to_mbr_function);
               fn::bind_back(ptr_to_data_mbr);
               fn::bind_back(fun_object);
          }
     }
}


SCENARIO("The bind_back function should take bound arguments by any value category.")
{
     GIVEN("A forwarding function an an lvalue, an xvalue, and an rvalue of an int")
     {
          auto fun = [] (auto&& arg) { return true; };
          int zero = 0;


          THEN("the bind_back function can be called with every value category of the int.")
          {
               fn::bind_back(fun, zero);
               fn::bind_back(fun, std::move(zero));
               fn::bind_back(fun, 0);
          }
     }
}


SCENARIO("Calling a bind_back_t object with arguments should invoke its bound function in a specific way.")
{
     GIVEN("A bind_back_t object with bound arguments")
     {
          int bound_arg  = 0;
          int called_arg = 0;
          auto fun = [] (int& arg1, int& arg2) { arg1 = 1; arg2 = 2; };

          auto bound_fun = fn::bind_back(fun, std::ref(bound_arg));


          WHEN("it is called with additional arguments")
          {
               bound_fun(called_arg);


               THEN("it should call the bound function with the calling arguments followed by the bound arguments")
               {
                    REQUIRE( bound_arg  == 2 );
                    REQUIRE( called_arg == 1 );
               }
          }
     }
}


SCENARIO("A bind_back_t object should take calling arguments to its child function by any value category.")
{
     GIVEN("A bound forwarding function and an lvalue, an xvalue, and an rvalue of an int")
     {
          auto fun = [] (auto&& arg) { return true; };
          auto bound_fun = fn::bind_back(fun);
          int zero = 0;


          THEN("the bound function can be called with each value category of int.")
          {
               bound_fun(zero);
               bound_fun(std::move(zero));
               bound_fun(0);
          }
     }
}


SCENARIO("A bind_back_t object should return the return value of its bound function when invoked.")
{
     auto fun = [] () { return true; };
     auto bound_fun = fn::bind_back(fun);

     REQUIRE( bound_fun() == true );
}


// TODO: cv qualifications and exceptions


// =====================================================================================================================
// Algorithms and combinators
// =====================================================================================================================
SCENARIO("An algorithm or combinator should allow its address to be taken without needing to specializing it.")
{
     auto fn_identity = fn::identity;
     auto fo_identity = fo::identity;

     auto fn_negate = fn::negate;
     auto fo_negate = fo::negate;

     auto fn_optional = fn::optional;
     auto fo_optional = fo::optional;

     auto fn_at_most = fn::at_most;
     auto fo_at_most = fo::at_most;

     auto fn_n_times = fn::n_times;
     auto fo_n_times = fo::n_times;

     auto fn_repeat = fn::repeat;
     auto fo_repeat = fo::repeat;

     auto fn_many = fn::many;
     auto fo_many = fo::many;

     auto fn_at_least = fn::at_least;
     auto fo_at_least = fo::at_least;

     auto fn_some = fn::some;
     auto fo_some = fo::some;

     auto fn_any = fn::any;
     auto fo_any = fo::any;

     auto fn_all = fn::all;
     auto fo_all = fo::all;
}


SCENARIO("An algorithm or combinator should take its child function by any value category.")
{
     GIVEN("an lvalue, an xvalue, and a prvalue of a function")
     {
          // Reference: https://en.cppreference.com/w/cpp/language/decltype

          auto returns_true = [] () { return true; };


          THEN("the identity algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(returns_true)>   );
          }


          THEN("the negate algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(returns_true)>   );
          }


          THEN("the optional algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(returns_true)>   );
          }


          THEN("the at_most algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(returns_true)>   );
          }


          THEN("the n_times algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(returns_true)>   );
          }


          THEN("the repeat algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(returns_true)>   );
          }


          THEN("the many algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(returns_true)>   );
          }


          THEN("the at_least algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(returns_true)>   );
          }


          THEN("the some algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(returns_true)>   );
          }


          THEN("the any algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(returns_true)>   );
          }


          THEN("the all algorithm or combinator can be called with each one.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(returns_true)>   );

               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(returns_true)&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(returns_true)&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(returns_true)>   );
          }
     }
}


SCENARIO("An algorithm or combinator should take arguments to its child function by any value category.")
{
     GIVEN("A forwarding function and an lvalue, an xvalue, and an rvalue of an int")
     {
          auto fun = [] (auto&& arg) { return true; };


          THEN("the identity algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::identity(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::identity(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity(fun)), int>   );
          }


          THEN("the negate algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::negate(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::negate(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate(fun)), int>   );
          }


          THEN("the optional algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::optional(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::optional(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional(fun)), int>   );
          }


          THEN("the at_most algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::at_most(1, fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most(1, fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most(1, fun)), int>   );
          }


          THEN("the n_times algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::n_times(1, fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times(1, fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times(1, fun)), int>   );
          }


          THEN("the repeat algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::repeat(1, 1, fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat(1, 1, fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat(1, 1, fun)), int>   );
          }


          THEN("the many algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::many(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::many(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::many(fun)), int>   );
          }


          THEN("the at_least algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::at_least(1, fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least(1, fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least(1, fun)), int>   );
          }


          THEN("the some algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun), int>   );

               REQUIRE( std::is_invocable_v<decltype(fo::some(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::some(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::some(fun)), int>   );
          }


          THEN("the any algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::any), std::tuple<int>&,  decltype(fun)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), std::tuple<int>&&, decltype(fun)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), std::tuple<int>,   decltype(fun)> );

               REQUIRE( std::is_invocable_v<decltype(fo::any(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::any(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::any(fun)), int>   );
          }


          THEN("the all algorithm or combinator can be called with the function and each value category of int.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::all), std::tuple<int>&,  decltype(fun)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), std::tuple<int>&&, decltype(fun)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), std::tuple<int>,   decltype(fun)> );

               REQUIRE( std::is_invocable_v<decltype(fo::all(fun)), int&>  );
               REQUIRE( std::is_invocable_v<decltype(fo::all(fun)), int&&> );
               REQUIRE( std::is_invocable_v<decltype(fo::all(fun)), int>   );
          }
     }
}


SCENARIO("An algorithm or combinator should be invocable with any callable type that returns a type contextually "
         "convertible to bool.")
{
     GIVEN("Every type of callable object")
     {
          // Although instances of every type are not explictly needed for the tests, it helps with comprehension.
          bool (*fun_ptr) () = [] () { return true; };
          auto closure = [] () { return true; };

          struct a_class
          {
               bool fun () { return true; }

               // Data members are callable, see https://en.cppreference.com/w/cpp/named_req/Callable
               bool data;
          };

          auto ptr_to_mbr_function = &a_class::fun;
          auto ptr_to_data_mbr = &a_class::data;

          struct fun_object_t
          {
               bool operator() () { return true; }
          } fun_object;


          THEN("the identity algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(fun_object)> );
          }


          THEN("the negate algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(fun_object)> );
          }


          THEN("the optional algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(fun_object)> );
          }


          THEN("the at_most algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(fun_object)> );
          }


          THEN("the n_times algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(fun_object)> );
          }


          THEN("the repeat algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(fun_object)> );
          }


          THEN("the many algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(fun_object)> );
          }


          THEN("the at_least algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(fun_object)> );
          }


          THEN("the some algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(fun_object)> );
          }


          THEN("the any algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(fun_object)> );
          }


          THEN("the all algorithm and combinator can be called with each object.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(closure)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(fun_object)> );
          }
     }


     GIVEN("A function which returns a non-boolean type that is contextually convertible to bool")
     {
          struct non_boolean_t {
               operator bool () { return true; }
          };

          auto return_non_bool = [] () -> non_boolean_t { return {}; };

          THEN("the algorithm and combinator can be called with the function.")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(return_non_bool)> );

               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(return_non_bool)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(return_non_bool)> );
          }
     }
}


SCENARIO("Invoking an algorithm or combinator should invoke the child function with the calling arguments.")
{
     GIVEN("A boolean_invocable function object taking an argument")
     {
          bool was_invoked1 = false;
          bool was_invoked2 = false;

          // Some algorithms will run forever if they don't return false.
          int count1 = 0;
          int count2 = 0;

          auto fun = [] (bool& arg, int& count) -> bool
          {
               if (count == 3)    return false;
               ++count;

               arg = true;
               return true;
          };


          WHEN("the identity algorithm and combinator is called with the function and the argument")
          {
               fn::identity(fun, was_invoked1, count1);
               fo::identity(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the negate algorithm and combinator is called with the function and the argument")
          {
               fn::negate(fun, was_invoked1, count1);
               fo::negate(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the optional algorithm and combinator is called with the function and the argument")
          {
               fn::optional(fun, was_invoked1, count1);
               fo::optional(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the at_most algorithm and combinator is called with the function and the argument")
          {
               fn::at_most(1, fun, was_invoked1, count1);
               fo::at_most(1, fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the n_times algorithm and combinator is called with the function and the argument")
          {
               fn::n_times(1, fun, was_invoked1, count1);
               fo::n_times(1, fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the repeat algorithm and combinator is called with the function and the argument")
          {
               fn::repeat(1, 1, fun, was_invoked1, count1);
               fo::repeat(1, 1, fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the many algorithm and combinator is called with the function and the argument")
          {
               fn::many(fun, was_invoked1, count1);
               fo::many(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the at_least algorithm and combinator is called with the function and the argument")
          {
               fn::at_least(1, fun, was_invoked1, count1);
               fo::at_least(1, fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the some algorithm and combinator is called with the function and the argument")
          {
               fn::some(fun, was_invoked1, count1);
               fo::some(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the any algorithm and combinator is called with the function and the argument")
          {
               fn::any(std::make_tuple(std::ref(was_invoked1), std::ref(count1)), fun);
               fo::any(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }


          WHEN("the all algorithm and combinator is called with the function and the argument")
          {
               fn::all(std::make_tuple(std::ref(was_invoked1), std::ref(count1)), fun);
               fo::all(fun)(was_invoked2, count2);


               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }
     }
}


SCENARIO("Every algorithm and combinator should return a specific boolean result under certain conditions.")
{
     GIVEN("One function object that returns true and one that returns false")
     {
          auto returns_true  = [] () { return true;  };
          auto returns_false = [] () { return false; };


          THEN("the identity algorithm and combinator should return the same result as its child function.")
          {
               REQUIRE( fn::identity(returns_true)    == true  );
               REQUIRE( fn::identity(returns_false)   == false );

               REQUIRE( fo::identity(returns_true)()  == true  );
               REQUIRE( fo::identity(returns_false)() == false );
          }


          THEN("the negate algorithm and combinator should return the negated result of its child function.")
          {
               REQUIRE( fn::negate(returns_true)    == false );
               REQUIRE( fn::negate(returns_false)   == true  );

               REQUIRE( fo::negate(returns_true)()  == false );
               REQUIRE( fo::negate(returns_false)() == true  );
          }


          THEN("the optional algorithm and combinator should always return true.")
          {
               REQUIRE( fn::optional(returns_true)    == true );
               REQUIRE( fn::optional(returns_false)   == true );

               REQUIRE( fo::optional(returns_true)()  == true );
               REQUIRE( fo::optional(returns_false)() == true );
          }


          THEN("the at_most algorithm and combinator should return always return true.")
          {
               REQUIRE( fn::at_most(1, returns_true)    == true );
               REQUIRE( fn::at_most(1, returns_false)   == true );

               REQUIRE( fo::at_most(1, returns_true)()  == true );
               REQUIRE( fo::at_most(1, returns_false)() == true );
          }


          THEN("the many algorithm and combinator should return always return true.")
          {
               // Cannot test a returns_true function here, since the algorithm will never stop. See testing of side
               // effects for assurance that the algorithm continues even when a function returns true.

               REQUIRE( fn::many(returns_false)   == true );
               REQUIRE( fo::many(returns_false)() == true );
          }


          WHEN("the any algorithm and combinator are called with the functions")
          {
               THEN("they should return true if at least one of the functions returns true, but not all.")
               {
                    REQUIRE( fn::any(returns_false, returns_true)   == true );
                    REQUIRE( fo::any(returns_false, returns_true)() == true );
               }


               THEN("they should return false if none of the functions returns true.")
               {
                    REQUIRE( fn::any(returns_false)   == false );
                    REQUIRE( fo::any(returns_false)() == false );
               }
          }


          WHEN("the all algorithm and combinator are called with the functions")
          {
               THEN("they should return true if all of the functions returns true.")
               {
                    REQUIRE( fn::all(returns_true)   == true );
                    REQUIRE( fo::all(returns_true)() == true );
               }


               THEN("they should return false if at least one of the functions returns false, but not all.")
               {
                    REQUIRE( fn::all(returns_true, returns_false)   == false );
                    REQUIRE( fo::all(returns_true, returns_false)() == false );
               }
          }
     }


     GIVEN("A function object which returns true for a certain amount of times it is called, then returns false")
     {
          int count1;
          int count2;

          auto counts_to_m = [] (int m, int& count)
          {
               if (count == m)     return false;

               ++count;
               return true;
          };


          WHEN("the n_times algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("they should return true if the function has returned true at least n times.")
               {
                    REQUIRE( fn::n_times(4, counts_to_m, 5, count1) == true  );
                    REQUIRE( fo::n_times(4, counts_to_m)(5, count2) == true  );
               }


               THEN("they should return false if the function has not returned true at least n times.")
               {
                    REQUIRE( fn::n_times(6, counts_to_m, 5, count1) == false );
                    REQUIRE( fo::n_times(6, counts_to_m)(5, count2) == false );
               }
          }


          WHEN("the repeat algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("they should return true if the function returns true after it has been called at least min "
                    "times.")
               {
                    REQUIRE( fn::repeat(4, 7, counts_to_m, 5, count1) == true);
                    REQUIRE( fo::repeat(4, 7, counts_to_m)(5, count2) == true);
               }


               THEN("they should return false if the function returns false before it has been called at least min "
                    "times.")
               {
                    REQUIRE( fn::repeat(6, 7, counts_to_m, 5, count1) == false);
                    REQUIRE( fo::repeat(6, 7, counts_to_m)(5, count2) == false);
               }
          }


          WHEN("the at_least algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("they should return true if the function returns true after it has been called at least n times.")
               {
                    REQUIRE( fn::at_least(4, counts_to_m, 5, count1) == true);
                    REQUIRE( fo::at_least(4, counts_to_m)(5, count2) == true);
               }


               THEN("they should return false if the function returns false before it has been called at least n "
                    "times.")
               {
                    REQUIRE( fn::at_least(6, counts_to_m, 5, count1) == false);
                    REQUIRE( fo::at_least(6, counts_to_m)(5, count2) == false);
               }
          }


          WHEN("the some algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("they should return true if the function returns true after it has been called at least 1 time.")
               {
                    REQUIRE( fn::some(counts_to_m, 1, count1) == true );
                    REQUIRE( fo::some(counts_to_m)(1, count2) == true );
               }


               THEN("they should return false if the function returns false on its first call.")
               {
                    REQUIRE( fn::some(counts_to_m, 0, count1) == false );
                    REQUIRE( fo::some(counts_to_m)(0, count2) == false );
               }
          }
     }
}


SCENARIO("Every algorithm and combinators should call a function a specific number of times.")
{
     GIVEN("A function object which returns true until it has been called m times, then returns false")
     {
          int count1;
          int count2;

          auto counts_to_m = [] (int m, int& count)
          {
               if (count == m)     return false;

               ++count;
               return true;
          };


          WHEN("the identity algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count only once if m > 1.")
               {
                    fn::identity(counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::identity(counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the negate algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count only once if m > 1.")
               {
                    fn::negate(counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::negate(counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the optional algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count only once if m > 1.")
               {
                    fn::optional(counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::optional(counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the at_most algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count n times if m > n.")
               {
                    fn::at_most(1, counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::at_most(1, counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }


               THEN("the function should count n times if m == n.")
               {
                    fn::at_most(2, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::at_most(2, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if m < n.")
               {
                    fn::at_most(3, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::at_most(3, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }
          }


          WHEN("the n_times algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count n times if m > n.")
               {
                    fn::n_times(1, counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::n_times(1, counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }


               THEN("the function should count n times if m == n.")
               {
                    fn::n_times(2, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::n_times(2, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if m < n.")
               {
                    fn::n_times(3, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::n_times(3, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }
          }


          WHEN("the repeat algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count m times if m < min <= max.")
               {
                    fn::repeat(3, 3, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::repeat(3, 3, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if min == m <= max.")
               {
                    fn::repeat(2, 2, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::repeat(2, 2, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if min < m < max.")
               {
                    fn::repeat(1, 3, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::repeat(1, 3, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if min < m <= max.")
               {
                    fn::repeat(1, 2, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::repeat(1, 2, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count max times if min <= max < m.")
               {
                    fn::repeat(1, 1, counts_to_m, 2, count1);
                    REQUIRE( count1 == 1 );

                    fo::repeat(1, 1, counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the many algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should not count if m == 0.")
               {
                    fn::many(counts_to_m, 0, count1);
                    REQUIRE( count1 == 0 );

                    fo::many(counts_to_m)(0, count1);
                    REQUIRE( count1 == 0 );
               }


               THEN("the function should count m times if m > 0.")
               {
                    fn::many(counts_to_m, 1, count1);
                    REQUIRE( count1 == 1 );

                    fo::many(counts_to_m)(1, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the at_least algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should count m times if m > n.")
               {
                    fn::at_least(1, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::at_least(1, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if m == n.")
               {
                    fn::at_least(2, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::at_least(2, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }


               THEN("the function should count m times if m < n.")
               {
                    fn::at_least(3, counts_to_m, 2, count1);
                    REQUIRE( count1 == 2 );

                    fo::at_least(3, counts_to_m)(2, count2);
                    REQUIRE( count2 == 2 );
               }
          }


          WHEN("the some algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should not count if m == 0.")
               {
                    fn::some(counts_to_m, 0, count1);
                    REQUIRE( count1 == 0 );

                    fo::some(counts_to_m)(0, count1);
                    REQUIRE( count1 == 0 );
               }


               THEN("the function should count m times if m > 0.")
               {
                    fn::some(counts_to_m, 1, count1);
                    REQUIRE( count1 == 1 );

                    fo::some(counts_to_m)(1, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the any algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should only count once if m > 1.")
               {
                    fn::any(std::make_tuple(2, std::ref(count1)), counts_to_m);
                    REQUIRE( count1 == 1 );

                    fo::any(counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }


          WHEN("the all algorithm and combinator are called with the function")
          {
               count1 = 0;
               count2 = 0;


               THEN("the function should only count once if m > 1.")
               {
                    fn::all(std::make_tuple(2, std::ref(count1)), counts_to_m);
                    REQUIRE( count1 == 1 );

                    fo::all(counts_to_m)(2, count2);
                    REQUIRE( count2 == 1 );
               }
          }
     }
}
