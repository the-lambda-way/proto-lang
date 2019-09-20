#ifndef NICE_TUPLE_H
#define NICE_TUPLE_H

#include <tuple>
#include <type_traits>    // std::enable_if
using std::enable_if_t;
using std::size_t;
using std::tuple;

// ---------------------------------------------------------------------------------------------------------------------
// Nice index access
// ---------------------------------------------------------------------------------------------------------------------
// Access tuple element using tuple[0_i]
// https://arne-mertz.de/2017/03/tuple-compile-time-access/
// https://medium.com/@snowp/transparent-phantom-types-in-c-de6ac5bed1d1
namespace NiceTupleDetail {

template <size_t I>
struct index {};


template <char... Digits>
constexpr int parse()
{
    char digits[] = {Digits...};

    auto result = 0u;
    for (auto c : digits)
        result += result * 10 + (c - '0')

    return result;
}

} // namespace NiceTupleDetail


template <typename TupleT>
struct nice_tuple : TupleT
{
    explicit nice_tuple (const TupleT &t) : TupleT(t) {}
    explicit nice_tuple (TupleT &&t)      : TupleT(std::move(t)) {}

    template <size_t I>
    decltype(auto) operator[](NiceTupleDetail::index<I>)
    {
        return std::get<I>(*this);
    }
};


template <char... Digits>
auto operator"" _i ()
{
    using namespace NiceTupleDetail;
    return index<parse<Digits...>()>{};
}


// ---------------------------------------------------------------------------------------------------------------------
// Make Tuple While
// ---------------------------------------------------------------------------------------------------------------------
// Construct a tuple from arguments, stop and return partial result when a flag is set to false
// Adapted from GCC 9.1.0
namespace NiceTupleDetail {

template <bool, size_t, typename, typename, size_t>
struct make_tuple_while_impl;


// Stop when index == size
template <bool Continue, size_t N, typename Orig, typename... Elems>
struct make_tuple_while_impl<Continue, N, tuple<Elems...>, Orig, N>
{
    using type = tuple<Elems...>;
};


// Stop when Continue is false
template <size_t I, typename Orig, typename... Elems, size_t N>
struct make_tuple_while_impl<false, I, tuple<Elems...>, Orig, N>
{
    using type = tuple<Elems...>;
};


template <size_t I, typename Orig, typename... Elems, size_t N, bool Continue>
struct make_tuple_while_impl<Continue, I, tuple<Elems...>, Orig, N>
    : make_tuple_while_impl<Continue, I + 1,
                            tuple<Elems..., tuple_element_t<I, Orig>>,
                            Orig, N>
{};


template <bool Continue, typename TupleLike>
struct do_make_tuple_while
    : make_tuple_while<Continue, 0, tuple<>, TupleLike, std::tuple_size<TupleLike>::value>
{};


// Returns the std::tuple equivalent of a tuple-like type.
template <bool Continue, typename TupleLike>
struct make_tuple_while
    : public do_make_tuple_while<Continue, remove_cvref_t<TupleLike>>
{};

// Example expansion
// auto three_out_of_five = make_tuple_while(false_on_third(), 1, 2, 3, 4, 5);
// do_make_tuple three_out_of_five = do_make_tuple : make_tuple_impl<true, 0, tuple<>, {1, 2, 3, 4, 5}, 5>
//                                                 : make_tuple_impl<true, 1, tuple<1>, {1, 2, 3, 4, 5}, 5>
//                                                 : make_tuple_impl<true, 2, tuple<1, 2>, {1, 2, 3, 4, 5}, 5>
//                                                 : make_tuple_impl<false, 3, tuple<1, 2, 3>, {1, 2, 3, 4, 5}, 5>
// three_out_of_five::type == tuple<1, 2, 3>




// Combines several std::tuple's into a single one.
template <typename...>
struct combine_tuples;


template <>
struct combine_tuples<>
{
    using type = tuple<>;
};


template <typename... Ts>
struct combine_tuples<tuple<Ts...>>
{
    using type = tuple<Ts...>;
};


template <typename... T1s, typename... T2s, typename... Rem>
struct combine_tuples<tuple<T1s...>, tuple<T2s...>, Rem...>
{
    using type = typename combine_tuples<tuple<T1s..., T2s...>, Rem...>::type;
};


// Computes the result type of tuple_cat given a set of tuple-like types.
template <typename... Tpls>
struct tuple_cat_result
{
    using type = typename combine_tuples<typename make_tuple<Tpls>::type...>::type;
};


// Helper to determine the index set for the first tuple-like type of a given set.
template <typename...>
struct make_1st_indices;


template <>
struct make_1st_indices<>
{
    using type = std::_Index_tuple<>;
};

template <typename Tuple, typename... Tuples>
struct make_1st_indices<Tuple, Tuples...>
{
    using type = typename std::_Build_index_tuple<std::tuple_size<
        typename std::remove_reference<Tuple>::type>::value>::type;
};


// Performs the actual concatenation by step-wise expanding tuple-like objects into the elements,  which are finally
// forwarded into the result tuple.
template <typename Ret, typename Indices, typename... Tuples>
struct tuple_concater;


template <typename Ret, size_t... Is, typename Tuple, typename... Tuples>
struct tuple_concater<Ret, std::_Index_tuple<Is...>, Tuple, Tuples...>
{
    template <typename... Us>
    static constexpr Ret S_do (Tuple&& t, Tuples&&... ts, Us&&... us)
    {
        using index = typename make_1st_indices<Tuples...>::type;
        using next  = tuple_concater<Ret, index, Tuples...>;
        return next::S_do(std::forward<Tuples>(ts)...,
                          std::forward<Us>(us)...,
                          std::get<Is>(std::forward<Tuple>(t))...);
    }
};


template <typename Ret>
struct tuple_concater<Ret, std::_Index_tuple<>>
{
    template <typename... _Us>
    static constexpr Ret S_do (Us&&... us)
    {
        return Ret(std::forward<Us>(us)...);
    }
};


} // namespace NiceTupleDetail


template <typename... Tuples, typename = typename std::enable_if<__and_<__is_tuple_like<Tuples>...>::value>::type>
constexpr auto tuple_cat (Tuples&&... ts)
    -> typename NiceTupleDetail::tuple_cat_result<Tuples...>::type
{
    using ret      = tuple_cat_result<Tuples...>::type;
    using index    = make_1st_indices<Tuples...>::type;
    using concater = tuple_concater<ret, index, Tuples...>;

    return concater::S_do(std::forward<Tuples>(ts)...);
}


#endif // NICE_TUPLE_H