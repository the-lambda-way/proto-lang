#ifndef MATCHER_GENERATORS
#define MATCHER_GENERATORS

#ifndef PL
#define PatLib PL
#endif

#ifndef PD
#define PatDef PD
#endif

#include <algorithm>
#include <functional>     // std::search searchers
#include <iostream>       // cout
#include <optional>
#include <string>
#include <string_view>
#include <tuple>          // ParseTree
#include <type_traits>    // is_same concept
#include <utility>        // std::pair, std::index_sequence

#include "../include/scanning-algorithms.h"
#include "../include/matching-algorithms.h"


using std::forward;
using std::make_tuple;
using std::move;
using std::string_view;
using std::tuple;


namespace PatLib {


/**
 * MAKE_MATCHER is a function-object-generator macro which can be used to bind arguments to an object that is later
 * called to output a string of characters that matches a given pattern.
 *
 * @param     params     Parameters to bind to the object upon instantiation
 * @param     out        Output container to contain the matching substring
 * @param     in         Mutable range representing a string
 * @param     first_in   Iterator to the start of a string
 * @param     last_in    Sentinel to the end of the string
 * @returns   Whether the scan was successful
 */
#define MAKE_MATCHER(class_name, function_name)                                                                  \
    template <typename... Parameters>                                                                            \
    class class_name                                                                                             \
    {                                                                                                            \
    public:                                                                                                      \
        class_name (Parameters... params) : parameters {move(params)...} {}                                      \
                                                                                                                 \
        template <typename Output>                                                                               \
        bool operator() (Output out, mutable_range& in)                                                          \
        {                                                                                                        \
            return range_impl(out, in, parameters, std::index_sequence_for<Parameters...>{});                    \
        }                                                                                                        \
                                                                                                                 \
        template <forward_iterator Iterator>                                                                     \
        bool operator() (Iterator& first_out, Iterator& last_out, mutable_range& in)                             \
        {                                                                                                        \
            return range_impl(first_out, last_out, in, parameters, std::index_sequence_for<Parameters...>{});    \
        }                                                                                                        \
                                                                                                                 \
        template <typename Output,                                                                               \
                  forward_iterator Iterator,                                                                     \
                  sentinel_for<Iterator> Sentinel>                                                               \
        bool operator() (Output out, Iterator& first_in, Sentinel last_in)                                       \
        {                                                                                                        \
            return iter_impl(out, first_in, last_in, parameters, std::index_sequence_for<Parameters...>{});      \
        }                                                                                                        \
                                                                                                                 \
        template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel>                                    \
        bool operator() (Iterator& first_out, Iterator& last_out,                                                \
                         Iterator& first_in, Sentinel last_in)                                                   \
        {                                                                                                        \
            return iter_impl(first_out, last_out, first_in, last_in,                                             \
                             parameters, std::index_sequence_for<Parameters...>{});                              \
        }                                                                                                        \
                                                                                                                 \
    private:                                                                                                     \
        const std::tuple<Parameters...> parameters;                                                              \
                                                                                                                 \
        template <typename Output, typename Tuple, std::size_t... I>                                             \
        bool range_impl (Output out, mutable_range& in, Tuple&& t, std::index_sequence<I...>)                    \
        {                                                                                                        \
            return match_with_if(out, in, function_name, std::get<I>(forward<Tuple>(t))...);                     \
        }                                                                                                        \
                                                                                                                 \
        template <forward_iterator Iterator, typename Tuple, std::size_t... I>                                   \
        bool range_impl (Iterator& first_out, Iterator& last_out,                                                \
                         mutable_range& in,                                                                      \
                         Tuple&& t, std::index_sequence<I...>)                                                   \
        {                                                                                                        \
            return match_with_if(first_out, last_out, in, function_name, std::get<I>(forward<Tuple>(t))...);     \
        }                                                                                                        \
                                                                                                                 \
        template <typename Output,                                                                               \
                  forward_iterator Iterator,                                                                     \
                  sentinel_for<Iterator> Sentinel,                                                               \
                  typename Tuple,                                                                                \
                  std::size_t... I>                                                                              \
        bool iter_impl (Output out,                                                                              \
                        Iterator& first_in, Sentinel last_in,                                                    \
                        Tuple&& t, std::index_sequence<I...>)                                                    \
        {                                                                                                        \
            return match_with_if(out, first_in, last_in, function_name, std::get<I>(std::forward<Tuple>(t))...); \
        }                                                                                                        \
                                                                                                                 \
        template <forward_iterator Iterator,                                                                     \
                  sentinel_for<Iterator> Sentinel,                                                               \
                  typename Tuple,                                                                                \
                  std::size_t... I>                                                                              \
        bool iter_impl (Iterator& first_out, Iterator& last_out,                                                 \
                        Iterator& first_in, Sentinel last_in,                                                    \
                        Tuple&& t, std::index_sequence<I...>)                                                    \
        {                                                                                                        \
            return match_with_if(first_out, last_out, first_in, last_in,                                         \
                                 function_name, std::get<I>(std::forward<Tuple>(t))...);                         \
        }                                                                                                        \
    };                                                                                                           \


namespace Match
{

namespace Detail
{
    MAKE_MATCHER(lit_t,         advance_if);
    MAKE_MATCHER(opt_t,         advance_optionally);
    MAKE_MATCHER(many_t,        advance_while);
    MAKE_MATCHER(while_not_t,   advance_while_not);
    MAKE_MATCHER(stop_before_t, advance_to_if_found);
    MAKE_MATCHER(until_t,       advance_past_if);
    MAKE_MATCHER(n_times_t,     advance_n_if);
    MAKE_MATCHER(at_least_t,    advance_min_if);
    MAKE_MATCHER(at_most_t,     advance_max_if);
    MAKE_MATCHER(rep_t,         advance_repeating);
    MAKE_MATCHER(any_t,         advance_any_if);
    MAKE_MATCHER(join_t,        advance_join_if);
}


// ---------------------------------------------------------------------------------------------------------------------
// Atomic matching
// ---------------------------------------------------------------------------------------------------------------------

template <typename Expr, typename... Args>
auto lit (Expr e, Args... args)
{
    return Detail::lit_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto opt (Expr e, Args... args)
{
    return Detail::opt_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto many (Expr e, Args... args)
{
    return Detail::many_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto while_not (Expr e, Args... args)
{
    return Detail::while_not_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto stop_before (Expr e, Args... args)
{
    return Detail::stop_before_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto until (Expr e, Args... args)
{
    return Detail::until_t(e, forward<Args>(args)...);
}


template <typename Expr, typename... Args>
auto n_times (size_t n, Expr e, Args... args)
{
    return Detail::n_times_t(e, forward<Args>(args)..., n);
}


template <typename Expr, typename... Args>
auto at_least (size_t n, Expr e, Args... args)
{
    return Detail::at_least_t(e, forward<Args>(args)..., n);
}


template <typename Expr, typename... Args>
auto at_most (size_t max, Expr e, Args... args)
{
    return Detail::at_most_t(e, forward<Args>(args)..., max);
}


template <typename Expr, typename... Args>
auto rep (size_t min, size_t max, Expr e, Args... args)
{
    return Detail::rep_t(e, forward<Args>(args)..., min, max);
}


template <typename... Expr>
auto any (Expr... e)
{
    return Detail::any_t(e...);
}


template <typename... Expr>
auto join (Expr... e)
{
    return Detail::join_t(e...);
}


// ---------------------------------------------------------------------------------------------------------------------
// Compound matching
// ---------------------------------------------------------------------------------------------------------------------


template <typename... Expr>
tuple<T...> tokenize_with (Expr... e)
{
    
}



} // namespace Match




} // namespace PatLib

#endif // MATCHER_GENERATORS
