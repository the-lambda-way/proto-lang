#ifndef CONCEPTS_KLUDGE_H
#define CONCEPTS_KLUDGE_H


// Shamelessly thrown together to help the compiler reason about generic code
// Once C++ provides STL concepts, these will be replaced
// Developed with reference to https://en.cppreference.com/w/cpp/concepts
// and with Niebler's rangesv3

#include <type_traits>
#include <iterator>       // std::iterator_traits
#include <utility>        // std::forward
#include "../include/detail/type_traits.hpp"

using std::forward;
using namespace concepts;    // type_traits.hpp


template <class T, class U>
concept bool same_as = std::is_same_v<T, U> && std::is_same_v<U, T>;


template <class From, class To>
concept bool convertible_to =
    std::is_convertible_v<From, To> &&
    requires (From (&f)())
    {
        static_cast<To>(f());
    };


template <class T, class U>
concept bool common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
    convertible_to<T, common_reference_t<T, U>> &&
    convertible_to<U, common_reference_t<T, U>>;


template <class T>
concept bool swappable =
    requires (T& a, T& b)
    {
        std::swap(a, b);
    };


template <class T, class U>
concept bool swappable_with =
    common_reference_with<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&> &&
    requires (T&& t, U&& u)
    {
        std::swap(forward<T>(t), forward<T>(t));
        std::swap(forward<U>(u), forward<U>(u));
        std::swap(forward<T>(t), forward<U>(u));
        std::swap(forward<U>(u), forward<T>(t));
    };




template <class LHS, class RHS>
concept bool assignable_from =
    std::is_lvalue_reference_v<LHS>          &&
    common_reference_with<
        const std::remove_reference_t<LHS>&,
        const std::remove_reference_t<RHS>&> &&
    requires (LHS lhs, RHS&& rhs)
    {
        { lhs = forward<RHS>(rhs) } -> LHS;
    };


template <class T>
concept bool destructible = std::is_nothrow_destructible_v<T>;


template <class T, class... Args>
concept bool constructible_from =
    destructible<T> && std::is_constructible_v<T, Args...>;


template <class T>
concept bool move_constructible =
    constructible_from<T, T> && convertible_to<T, T>;


template <class T>
concept bool movable =
    std::is_object_v<T>         &&
    move_constructible<T>  &&
    assignable_from<T&, T> &&
    swappable<T>;


template <class B>
concept bool boolean =
    movable<std::remove_cvref_t<B>> &&
    requires (const std::remove_reference_t<B>& b1,
              const std::remove_reference_t<B>& b2,
              const bool a)
    {
        { b1       } -> convertible_to<bool>;
        { !b1      } -> convertible_to<bool>;
        { b1 && b2 } -> bool;
        { b1 &&  a } -> bool;
        {  a && b2 } -> bool;
        { b1 || b2 } -> bool;
        { b1 ||  a } -> bool;
        {  a || b2 } -> bool;
        { b1 == b2 } -> convertible_to<bool>;
        { b1 ==  a } -> convertible_to<bool>;
        {  a == b2 } -> convertible_to<bool>;
        { b1 != b2 } -> convertible_to<bool>;
        { b1 !=  a } -> convertible_to<bool>;
        {  a != b2 } -> convertible_to<bool>;
    };


template <class T, class U>
concept bool weakly_equality_comparable_with =
    requires (const std::remove_reference_t<T>& t,
              const std::remove_reference_t<U>& u)
    {
        { t == u } -> boolean;
        { t != u } -> boolean;
        { u == t } -> boolean;
        { u != t } -> boolean;
    };


template <class T>
concept bool equality_comparable = weakly_equality_comparable_with<T, T>;


template <class T, class U>
concept bool equality_comparable_with =
    equality_comparable<T>                      &&
    equality_comparable<U>                      &&
    common_reference_with<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&>      &&
    equality_comparable<
        common_reference_t<
            const std::remove_reference_t<T>&,
            const std::remove_reference_t<U>&>> &&
    weakly_equality_comparable_with<T, U>;


template <class T>
concept bool destructible = std::is_nothrow_destructible_v<T>;


template <class T, class... Args>
concept bool constructible_from =
    destructible<T> && std::is_constructible_v<T, Args...>;


template <class T>
concept bool default_constructible = constructible_from<T>;


template <class T>
concept bool copy_constructible =
    move_constructible<T>                                          &&
    constructible_from<T, T&> && convertible_to<T&, T>             &&
    constructible_from<T, const T&> && convertible_to<const T&, T> &&
    constructible_from<T, const T> && convertible_to<const T, T>;


template <class T>
concept bool copyable =
    copy_constructible<T> &&
    movable<T>            &&
    assignable_from<T&, const T&>;


template <class T>
concept bool semiregular = copyable<T> && default_constructible<T>;


template <class T>
concept bool regular = semiregular<T> && equality_comparable<T>;


template <typename T>
using iter_difference_t = typename std::iterator_traits<T>::difference_type;


template <class I>
concept bool weakly_incrementable =
    default_constructible<I> &&
    movable<I>               &&
    requires (I i)
    {
        typename iter_difference_t<I>;
        { ++i } -> I&;
        i++;
    };


template <class I>
concept bool incrementable =
    regular<I>              &&
    weakly_incrementable<I> &&
    requires (I i)
    {
        { i++ } -> I;
    };


template <class T>
concept bool dereferenceable =
    requires (T t)
    {
        { *t };
    };


template <class T>
concept bool readable =
    requires (T t)
    {
        { *t } -> T;
    };

template <class T>
concept bool readable =
  requires
  {
    typename std::iterator_traits<T>::value_type;
    typename std::iterator_traits<T>::reference;
  } &&
  common_reference_with<typename std::iterator_traits<T>::reference&&, typename std::iterator_traits<T>::value_type&>;


template <class I, class T>
concept bool writeable =
    dereferenceable<I> &&
    requires (I&& i, T&& t)
    {
        *i = forward<T>(t);
        *forward<I>(i) = forward<T>(t);
    };


template <class I>
concept bool iterator =
    dereferenceable<I> && weakly_incrementable<I>;


template <class S, class I>
concept bool sentinel_for =
    semiregular<S> &&
    iterator<I>    &&
    weakly_equality_comparable_with<I, S>;


template <class S, class I>
concept bool sized_sentinel_for =
    sentinel_for<I, S> &&
    requires (I i, S s)
    {
        { i - s } -> iter_difference_t<I>;
        { s - i } -> iter_difference_t<I>;
    };


template <class I>
concept bool input_iterator = iterator<I> && readable<I>;


template <class I, class T>
concept bool output_iterator =
    iterator<I>     &&
    writeable<I, T> &&
    requires (I i, T&& t)
    {
        *i++ = std::forward<T>(t); // not required to be equality preserving
    };


template <class I>
concept bool forward_iterator =
    input_iterator<I>  &&
    incrementable<I>   &&
    sentinel_for<I, I>;


template <class I>
concept bool bidirectional_iterator =
    forward_iterator<I> &&
    requires (I i)
    {
        { --i } -> I&;
        { i-- } -> I&&;
    };


template <typename T>
concept bool totally_ordered =
    equality_comparable<T> &&
    requires (const T& t, const T& u)
    {
        requires t <  u;
        requires t >  u;
        requires u <= t;
        requires u >= t;
    };


template <class T>
using iter_reference_t = decltype(*std::declval<T&>());


template <class I>
concept bool random_access_iterator =
    bidirectional_iterator<I> &&
    totally_ordered<I>        &&
    requires (I i, typename std::iterator_traits<I>::difference_type n)
    {
        { i += n } -> I&;
        { i -= n } -> I&;
        { i +  n } -> I;
        { n +  i } -> I;
        { i -  n } -> I;
        { i -  i } -> same_as<decltype(n)>;
        {  i[n]  } -> convertible_to<iter_reference_t<I>>;
    };



#endif // CONCEPTS_KLUDGE_H
