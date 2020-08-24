#include <functional>     // std::ref
#include <utility>        // std::forward

#include "catch2/catch.hpp"
#include "pattern/fn-combinators.h"


using namespace Pattern;


// =====================================================================================================================
// fn::bind_back
// =====================================================================================================================




// =====================================================================================================================
// Algorithms
// =====================================================================================================================
// Since fn:: and fo:: function have similar behavior, we can save space by reusing the same SECTIONs. This also makes
// the tests easier to comprehend. That necessitates setting up separate state variables for each version.


TEST_CASE("All algorithms and combinators should allow their address to be taken without needing to specializing them.")
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


SCENARIO("Every algorithm and combinator should be specializable on any callable type that satisfies "
         "boolean_invocable.")
{
     GIVEN("Every type of callable object")
     {
          // Although instances of every type are not explictly needed for the tests, it helps with comprehension
          bool (*fun_ptr) () = [] () { return true; };
          auto lambda = [] () { return true; };

          struct a_class
          {
               bool fun () { return true; }

               // Data members are callable, see https://en.cppreference.com/w/cpp/named_req/Callable
               // Can be any type convertible to bool
               int data;
          };

          auto ptr_to_mbr_function = &a_class::fun;
          auto ptr_to_data_mbr = &a_class::data;

          struct fun_object_t
          {
               bool operator() () { return true; }
          } fun_object;

          THEN("the identity algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::identity), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::identity), decltype(fun_object)> );
          }

          THEN("the negate algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::negate), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::negate), decltype(fun_object)> );
          }

          THEN("the optional algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::optional), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::optional), decltype(fun_object)> );
          }

          THEN("the at_most algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_most), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_most), int, decltype(fun_object)> );
          }

          THEN("the n_times algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::n_times), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::n_times), int, decltype(fun_object)> );
          }

          THEN("the repeat algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::repeat), int, int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::repeat), int, int, decltype(fun_object)> );
          }

          THEN("the many algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::many), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::many), decltype(fun_object)> );
          }

          THEN("the at_least algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::at_least), int, decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::at_least), int, decltype(fun_object)> );
          }

          THEN("the some algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(ptr_to_mbr_function), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(ptr_to_data_mbr), a_class> );
               REQUIRE( std::is_invocable_v<decltype(fn::some), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::some), decltype(fun_object)> );
          }

          THEN("the any algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::any), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::any), decltype(fun_object)> );
          }

          THEN("the all algorithm and combinator can be called with each object")
          {
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all),
                        std::tuple<std::reference_wrapper<a_class>>,
                        decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fn::all), decltype(fun_object)> );

               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(fun_ptr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(lambda)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(ptr_to_mbr_function)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(ptr_to_data_mbr)> );
               REQUIRE( std::is_invocable_v<decltype(fo::all), decltype(fun_object)> );
          }
     }
}


SCENARIO("Invoking an algorithm or combinator should invoke the child function with the calling the arguments.")
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

          WHEN("the identity algorithm or combinator is called with the function and the argument")
          {
               fn::identity(fun, was_invoked1, count1);
               fo::identity(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the negate algorithm or combinator is called with the function and the argument")
          {
               fn::negate(fun, was_invoked1, count1);
               fo::negate(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the optional algorithm or combinator is called with the function and the argument")
          {
               fn::optional(fun, was_invoked1, count1);
               fo::optional(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the at_most algorithm or combinator is called with the function and the argument")
          {
               fn::at_most(1, fun, was_invoked1, count1);
               fo::at_most(1, fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the n_times algorithm or combinator is called with the function and the argument")
          {
               fn::n_times(1, fun, was_invoked1, count1);
               fo::n_times(1, fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the repeat algorithm or combinator is called with the function and the argument")
          {
               fn::repeat(1, 1, fun, was_invoked1, count1);
               fo::repeat(1, 1, fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the many algorithm or combinator is called with the function and the argument")
          {
               fn::many(fun, was_invoked1, count1);
               fo::many(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the at_least algorithm or combinator is called with the function and the argument")
          {
               fn::at_least(1, fun, was_invoked1, count1);
               fo::at_least(1, fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the some algorithm or combinator is called with the function and the argument")
          {
               fn::some(fun, was_invoked1, count1);
               fo::some(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the any algorithm or combinator is called with the function and the argument")
          {
               fn::any(std::make_tuple(std::ref(was_invoked1), std::ref(count1)), fun);
               fo::any(fun)(was_invoked2, count2);

               THEN("the function was invoked with the argument.")
               {
                    REQUIRE( was_invoked1 == true );
                    REQUIRE( was_invoked2 == true );
               }
          }

          WHEN("the all algorithm or combinator is called with the function and the argument")
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


// TODO: Add tests specific to each algorithm/combinator kind


