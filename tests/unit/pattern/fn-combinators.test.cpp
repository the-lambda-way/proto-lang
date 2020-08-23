#include <utility>     // std::forward

#include "catch2/catch.hpp"
#include "pattern/fn-combinators.h"


using namespace Pattern;


// =====================================================================================================================
// fn::bind_back
// =====================================================================================================================




// =====================================================================================================================
// Algorithms
// =====================================================================================================================
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


SCENARIO("Invoking the identity function or combinator should be equivalent to invoking its first argument with the "
         "rest of the arguments.")
{
     GIVEN("A boolean_invocable function object taking no arguments")
     {
          auto returns_true = [] () { return true; };

          WHEN("identity is called with the function")
          {
               auto val1 = fn::identity(returns_true);
               auto val2 = fo::identity(returns_true)();

               THEN("the return value from the function call is returned")
               {
                    REQUIRE( val1 == true );
                    REQUIRE( val2 == true );
               }
          }
     }

     GIVEN("A boolean_invocable function object taking an argument")
     {
          auto returns_arg  = [] (int i) { return i == 0; };

          WHEN("identity is called with the function and the argument")
          {
               auto val1 = fn::identity(returns_arg, 0);
               auto val2 = fo::identity(returns_arg)(0);

               THEN("the argument is passed to the function.")
               {
                    REQUIRE( val1 == true );
                    REQUIRE( val2 == true );
               }
          }
     }

     GIVEN("A boolean_invocable function object with a side effect")
     {
          int state1 = 0;
          int state2 = 0;
          auto increments = [] (int& i) { ++i; return true; };

          WHEN("identity is called with the function")
          {
               fn::identity(increments, state1);
               fo::identity(increments)(state2);

               THEN("the side effect occurred.")
               {
                    REQUIRE( state1 == 1 );
                    REQUIRE( state2 == 1 );
               }
          }
     }
}


SCENARIO("Invoking the negate function or combinator should be equivalent to invoking its first argument with the rest "
         "of the arguments and negating the result.")
{
     GIVEN("A boolean_invocable function object taking no arguments")
     {
          auto returns_true = [] () { return true; };

          WHEN("negate is called with the function")
          {
               auto val1 = fn::negate(returns_true);
               auto val2 = fo::negate(returns_true)();

               THEN("the return value from the function call is negated")
               {
                    REQUIRE( val1 == false );
                    REQUIRE( val2 == false );
               }
          }
     }

     GIVEN("A boolean_invocable function object taking an argument")
     {
          auto returns_arg  = [] (int i) { return i == 0; };

          WHEN("negate is called with the function and the argument")
          {
               auto val1 = fn::negate(returns_arg, 0);
               auto val2 = fo::negate(returns_arg)(0);

               THEN("the argument is passed to the function.")
               {
                    REQUIRE( val1 == false );
                    REQUIRE( val2 == false );
               }
          }
     }

     GIVEN("A boolean_invocable function object with a side effect")
     {
          int state1 = 0;
          int state2 = 0;
          auto increments = [] (int& i) { ++i; return true; };

          WHEN("negate is called with the function")
          {
               fn::negate(increments, state1);
               fo::negate(increments)(state2);

               THEN("the side effect occurred.")
               {
                    REQUIRE( state1 == 1 );
                    REQUIRE( state2 == 1 );
               }
          }
     }
}







