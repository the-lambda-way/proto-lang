3ifndef NICE_TUPLE_H
#define NICE_TUPLE_H

#include <tuple>
using std::tuple;
using size_t = unsigned long;    // G++9 definition


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
// Generate While
// ---------------------------------------------------------------------------------------------------------------------
// Generate a tuple, stop and return partial result when a flag is set to false

// (Not yet implemented)

// Adapted from GCC 9.1.0
namespace NiceTupleDetail {


template<size_t, typename, typename, size_t>
struct make_tuple_impl;


template<size_t Idx, typename Tuple, typename... Tp, size_t Nm>
struct make_tuple_impl<Idx, tuple<Tp...>, Tuple, Nm>
    : make_tuple_impl<Idx + 1,
                      tuple<Tp..., tuple_element_t<Idx, Tuple>>,
                      Tuple, Nm>
{};


template<size_t Nm, typename Tuple, typename... Tp>
struct make_tuple_impl<Nm, tuple<Tp...>, Tuple, Nm>
{
    using type = tuple<Tp...>;
};


template<typename Tuple>
struct do_make_tuple
    : make_tuple_impl<0, tuple<>, Tuple, std::tuple_size<Tuple>::value>
{};


// Returns the std::tuple equivalent of a tuple-like type.
template<typename Tuple>
struct make_tuple
    : public do_make_tuple<remove_cvref_t<Tuple>>
{};



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


template <typename... Tuples, typename = typename enable_if<__and_<__is_tuple_like<Tuples>...>::value>::type>
constexpr auto tuple_cat (Tuples&&... ts)
    -> typename NiceTupleDetail::tuple_cat_result<Tuples...>::type
{
    using ret      = tuple_cat_result<Tuples...>::type;
    using index    = make_1st_indices<Tuples...>::type;
    using concater = tuple_concater<ret, index, Tuples...>;

    return concater::S_do(std::forward<Tuples>(ts)...);
}


#undef // NICE_TUPLE_H