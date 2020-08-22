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


// TEST_CASE("Invoking the identity function or combinator should be equivalent to invoking its argument.")
// {


// }








