#ifndef SCANNER_GENERATORS
#define SCANNER_GENERATORS

#include <cstring>    // strlen
#include <tuple>      // scanner-type parameters
#include <utility>    // scanner-type index_sequence
#include "../include/scanning-algorithms.h"

using std::forward;
using std::move;

#ifndef PL
#define PatLib PL
#endif

using namespace Pattern;
using std::forward_iterator;
using std::sentinel_for;


namespace PatLib {

/**
 * MAKE_SCANNER is a function-object-generator macro which can be used to bind arguments to an object that is later
 * called to scan a string of characters.
 *
 * @param     params   Parameters to bind to the object upon instantiation
 * @param     r        Mutable range representing a string
 * @param     first    Iterator to the start of a string
 * @param     last     Sentinel to the end of the string
 * @returns   Whether the scan was successful
 */
#define MAKE_SCANNER(class_name, function_name)                                                  \
    template <typename... Parameters>                                                            \
    class class_name                                                                             \
    {                                                                                            \
    public:                                                                                      \
        class_name (Parameters... params) : parameters {move(params)...} {}                      \
                                                                                                 \
        bool operator() (mutable_range auto& r)                                                  \
        {                                                                                        \
            return range_impl(r, parameters, std::index_sequence_for<Parameters...>{});          \
        }                                                                                        \
                                                                                                 \
        template <forward_iterator Iterator, sentinel_for<Iterator> Sentinel>                    \
        bool operator() (Iterator& first, Sentinel last)                                         \
        {                                                                                        \
            return iter_impl(first, last, parameters, std::index_sequence_for<Parameters...>{}); \
        }                                                                                        \
                                                                                                 \
    private:                                                                                     \
        const std::tuple<Parameters...> parameters;                                              \
                                                                                                 \
        template <typename Tuple, std::size_t... I>                                              \
        bool range_impl (mutable_range auto& r, Tuple&& t, std::index_sequence<I...>)            \
        {                                                                                        \
            return function_name(r, std::get<I>(forward<Tuple>(t))...);                          \
        }                                                                                        \
                                                                                                 \
        template <forward_iterator Iterator,                                                     \
                  sentinel_for<Iterator> Sentinel,                                               \
                  typename Tuple,                                                                \
                  std::size_t... I>                                                              \
        bool iter_impl(Iterator& first, Sentinel last, Tuple&& t, std::index_sequence<I...>)     \
        {                                                                                        \
            return function_name(first, last, std::get<I>(std::forward<Tuple>(t))...);           \
        }                                                                                        \
    };                                                                                           \


namespace Scan
{

namespace Detail
{
    MAKE_SCANNER(lit_t,         advance_if);
    MAKE_SCANNER(opt_t,         advance_optionally);
    MAKE_SCANNER(many_t,        advance_while);
    MAKE_SCANNER(while_not_t,   advance_while_not);
    MAKE_SCANNER(stop_before_t, advance_to_if_found);
    MAKE_SCANNER(until_t,       advance_past_if);
    MAKE_SCANNER(n_times_t,     advance_n_if);
    MAKE_SCANNER(at_least_t,    advance_min_if);
    MAKE_SCANNER(at_most_t,     advance_max_if);
    MAKE_SCANNER(rep_t,         advance_repeating);
    MAKE_SCANNER(any_t,         advance_any_if);
    MAKE_SCANNER(join_t,        advance_join_if);
}

#undef MAKE_SCANNER


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


} // namespace Scan

} // namespace PatLib


#endif // SCANNER_GENERATORS
