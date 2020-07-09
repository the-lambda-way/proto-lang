/**
 * @file
 * @author Mike Castillo
 *
 * @section LICENSE
 *
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * @section DESCRIPTION
 *
 * Algorithms for scanning character sequences. Designed for the typical needs of scanning source code and creating
 * scanner abstractions.
 *
 * Rationale:
 * The Standard Library provides a few basic operations on character sequences, but they are not very suitable as a
 * basis for scanning applications. Algorithms are defined as class methods, preventing their generic use. In addition,
 * element traversal is separated from inspection. In scanning operations, these two concerns are tightly coupled and
 * provide a chance for optimization.
 *
 * The <algorithm> library overlaps somewhat with this one. This library provides a more useful interface for scanning,
 * as well as a larger number of scanning-related features.
 */

#pragma once

#include <algorithm>      // find
#include <concepts>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>    // is_reference_v, remove_reference


namespace Pattern {
namespace internal {

using namespace std;
using ranges::range;

namespace exports {


// ---------------------------------------------------------------------------------------------------------------------
//  Concepts
// ---------------------------------------------------------------------------------------------------------------------
template <class I>
concept input_iterator_ref = is_reference_v<I> && input_iterator<remove_reference<I>>;


template <class I>
using indirect_value_t = iter_value_t<remove_reference<I>>;


template <class E, class I>
concept atomic_scannable_expression = input_iterator_ref<I> && (
    equality_comparable_with<E, indirect_value_t<I>> ||
    indirect_unary_predicate<E, remove_reference<I>>);


template <class P, class I, class S>
concept scanning_algorithm =
    input_iterator_ref<I> && sentinel_for<remove_reference<I>, S> &&
    predicate<P, I, S>;


template <class E, class I, class S>
concept compound_scannable_expression =
    input_iterator_ref<I> && sentinel_for<remove_reference<I>, S> &&
    ((range<E> && equality_comparable_with<ranges::range_value_t<E>, indirect_value_t<I>)) ||
    scanning_algorithm<E, I, S>);


template <class E, class Iterator, class Sentinel>
concept scannable_expression =
    atomic_scannable_expression<E, Iterator> ||
    compound_scannable_expression<E, Iterator, Sentinel>;


template <class R>
concept mutable_range = ranges::input_range<R> && is_reference_v<ranges::iterator_t<R>>;


// ---------------------------------------------------------------------------------------------------------------------
// Scanning Predicates
// ---------------------------------------------------------------------------------------------------------------------
template <input_iterator I, sentinel_for<I> S, class T>
    requires equality_comparable_with<T, iter_value_t<I>>
bool starts_with (I first, S last, T t)
{
    return (first != last && *first == t);
}


template <input_iterator I, sentinel_for<I> S, indirect_unary_predicate<I> P>
bool starts_with (I first, S last, P p)
{
    return (first != last && invoke(p, *first));
}


template <class... Ts>
bool starts_with (range r, Ts&&... ts)
{
    return starts_with(r.begin(), r.end(), forward<Ts>(ts)...);
}


// ---------------------------------------------------------------------------------------------------------------------
// Element Traversal
// ---------------------------------------------------------------------------------------------------------------------
template <input_iterator I, sentinel_for<I> S>
bool scan_impl (I& first, S last, atomic_scannable_expression<I> e)
{
    if (!starts_with(first, last, e))    return false;
    ++first;
    return true;
}


template <input_iterator I1, sentinel_for<I1> S1, input_iterator I2, sentinel_for<I2> S2>
bool scan_impl (I1& first1, S1 last1, I2 first2, S2 last2)
{
    auto ptrs = mismatch(first1, last1, first2, last2);

    if (ptrs.second != last2)    return false;

    first1 = ptrs.first;
    return true;
}


template <input_iterator I, sentinel_for<I> S>
bool scan_impl (I& first, S last, range r)
{
    return scan_impl(first, last, r.begin(), r.end());
}


template <input_iterator I, sentinel_for<I> S>
bool scan_impl (I& first, S last, scanning_algorithm<I, S> f)
{
    return invoke(f, first, last);
}


template <class... Ts>
bool scan_impl (mutable_range& r, Ts&&... ts)
{
    return scan_impl(r.begin(), r.end(), forward<Ts>(ts)...);
}




// ---------------------------------------------------------------------------------------------------------------------
// Algorithms
// ---------------------------------------------------------------------------------------------------------------------
/**
 * Advances an iterator if a string satisfies an atomic scanning algorithm.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether the predicate returned true
 */
template <input_iterator I, sentinel_for<I> S, class... Args>
bool advance_if (I& first, S last, atomic_scannable_expression<Args...> e, Args&&... args)
{
    if (!scan(first, last, e, forward<Args>(args)...))    return false;
    ++first;
    return true;
}


/**
 * Advances an iterator if a string satisfies a compound scanning algorithm.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether literal compared equal
 */
template <input_iterator I, sentinel_for<I> S, class... Args,
          compound_scannable_expression<I, S, Args...> Expression>
bool advance_if (I& first, S last,
                           Expression e, Args&&... args)
{
    Iterator copy = first;

    if (!scan(copy, last, e, forward<Args>(args)...))    return false;

    first = copy;
    return true;
}


/**
 * Advances an iterator once if a string doesn't satisfy a given scanning algorithm.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   True if the scan failed
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_if_not (Iterator& first, Sentinel last,
                               Expression e, Args&&... args)
{
    if (scan(first, last, e, forward<Args>(args)...))    return false;
    ++first;
    return true;
}


/**
 * Calls a scanning algorithm, then returns true regardless of the result.
 *
 * @param    first     Iterator to the start of the string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Always returns true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_optionally (Iterator& first, Sentinel last,
                                   Expression e, Args&&... args)
{
    advance_if(first, last, e, forward<Args>(args)...);
    return true;
}


/**
 * Advances an iterator while a string begins with another string.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Always returns true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_while (Iterator& first, Sentinel last,
                              Expression e, Args... args)
{
    while (advance_if(first, last, e, args...));
    return true;
}


/**
 * Advances an iterator while a string satisfies a scanning algorithm, up to a maximum amount.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    max       Maximum number of times to repeat, -1 for infinite match
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Always returns true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          atomic_scannable_expression<Args...> Expression>
bool advance_max_if (Iterator& first, Sentinel last,
                               Expression e, Args&&... args,
                               size_t max = -1)
{
    // No algorithm can advance more times than the number of characters remaining (in theory)
    // *Sentinel* might not be convertible to *Iterator*, so a few extra steps are required
    size_t diff = last - first;
    last = last - diff + min(diff, max);

    advance_while(first, last, e, forward<Args>(args)...);
    return true;
}


/**
 * Advances an iterator while a string satisfies a scanning algorithm, up to a maximum amount.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    max       Maximum number of times to repeat, -1 for infinite match
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Always returns true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          compound_scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_max_if (Iterator& first, Sentinel last,
                               Expression e, Args&&... args,
                               size_t max = -1)
{
    while (max-- && advance_if(first, last, e, forward<Args>(args)...));
    return true;
}


/**
 * Advances an iterator while a string satisfies a scannable expression, exactly n times.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    n         Number of times to repeat
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether n repeats were successful
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_n_if (Iterator& first, Sentinel last,
                             Expression e, Args&&... args,
                             size_t n)
{
    if (first - last < n)    return false;
    if (n == 0)              return true;
    if (n == 1)              return advance_if(first, last, e, forward<Args>(args)...);

    Iterator copy = first;

    while (n--)
        if (!advance_if(copy, last, e, forward<Args>(args)...))    return false;

    first = copy;
    return true;
}


/**
 * Calls a series of scanning algorithms, returning true if any of them are satisfied.
 *
 * @param    first   Iterator to the start of a string
 * @param    last    Sentinel to the end of the string
 * @param    e...    Expressions to test the string with
 * @return   Whether any of the expressions returned true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          scannable_expression<Iterator, Sentinel>... Expression>
bool advance_if_any (Iterator& first, Sentinel last,
                               Expression... e)
{
    return (... || advance_if(first, last, e));
}


/**
 * Calls a series of scanning algorithms in order, returning true if all of them are satisfied.
 *
 * @param    first   Iterator to the start of a string
 * @param    last    Sentinel to the end of the string
 * @param    e...    Expressions to test the string with
 * @return   Whether all of the expressions returned true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          scannable_expression<Iterator, Sentinel>... Expression>
bool advance_join_if (Iterator& first, Sentinel last,
                                Expression... e)
{
    Iterator copy = first;

    if (!(... && advance_if(copy, last, e)))    return false;

    first = copy;
    return true;
}


// ---------------------------------------------------------------------------------------------------------------------
// Ranged Versions
// ---------------------------------------------------------------------------------------------------------------------


/**
 * Range-based overload of *advance_if*
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_if*
 * @return   The return value of the call to *advance_if*
 */
template <class... Args>
bool advance_if (mutable_range& r, Args&&... args)
{
    return advance_if(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_if_not*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_if_not*
 * @return   The return value of the call to *advance_if_not*
 */
template <class... Args>
bool advance_if_not (mutable_range& r, Args&&... args)
{
    return advance_if_not(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_optionally*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_optionally*
 * @return   The return value of the call to *advance_optionally*
 */
template <class... Args>
bool advance_optionally (mutable_range& r, Args&&... args)
{
    return advance_optionally(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_while*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_while*
 * @return   The return value of the call to *advance_while*
 */
template <class... Args>
bool advance_while (mutable_range& r, Args&&... args)
{
    return advance_while(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_max_if*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_max_if*
 * @return   The return value of the call to *advance_max_if*
 */
template <class... Args>
bool advance_max_if (mutable_range& r, Args&&... args)
{
    return advance_max_if(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_n_if*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_n_if*
 * @return   The return value of the call to *advance_n_if*
 */
template <class... Args>
bool advance_n_if (mutable_range& r, Args&&... args)
{
    return advance_n_if(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_if_any*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_if_any*
 * @return   The return value of the call to *advance_if_any*
 */
template <class... Args>
bool advance_if_any (mutable_range& r, Args&&... args)
{
    return advance_if_any(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_join_if*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_join_if*
 * @return   The return value of the call to *advance_join_if*
 */
template <class... Args>
bool advance_join_if (mutable_range& r, Args&&... args)
{
    return advance_join_if(r.begin(), r.end(), forward<Args>(args)...);
}


// ---------------------------------------------------------------------------------------------------------------------
// Derived Algorithms
// ---------------------------------------------------------------------------------------------------------------------
// These should be tested to check if they are redundant

/**
 * Advances an iterator while the beginning of a string doesn't satisfy a given scannable expression.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Always returns true
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_while_not (Iterator& first, Sentinel last,
                                  Expression e, Args... args)
{
    while (advance_if_not(first, last, e, forward<Args>(args)...));
    return true;
}


/**
 * Range-based overload of *advance_while_not*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_while_not*
 * @return   The return value of the call to *advance_while_not*
 */
template <class... Args>
bool advance_while_not (mutable_range& r, Args&&... args)
{
    return advance_while_not(r.begin(), r.end(), forward<Args>(args)...);
}


// If I can create a repeat algorithm that optimizes down to its parts, then at_least(), at_most(), n_times(), and
// optional() can be combined into one

/**
 * Advances an iterator while a string satisfies a scanning algorithm, between min and max times.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    min       Minimum number of times to repeat
 * @param    max       Maximum number of times to repeat, -1 to match until fail
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether at least *min* repeats were successful
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_repeating (Iterator& first, Sentinel last,
                                  Expression e, Args... args,
                                  size_t min = 0, size_t max = -1)
{
    if (max < min)    return false;

    if (!advance_n_if(first, last, e, args..., min))    return false;
    advance_max_if(first, last, e, args..., max - min);
    return true;
}


/**
 * Range-based overload of *advance_repeating*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_repeating*
 * @return   The return value of the call to *advance_repeating*
 */
template <class... Args>
bool advance_repeating (mutable_range& r, Args&&... args)
{
    return advance_repeating(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Advances an iterator while a string satisfies a scanning algorithm, a minimum number of times.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    min       Minimum number of times to repeat
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether at least *min* repeats were successful
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_min_if (Iterator& first, Sentinel last,
                               Expression e, Args... args,
                               size_t min = 0)
{
    if (!advance_n_if(first, last, e, args..., min))    return false;
    advance_while(first, last, e, args...);
    return true;
}


/**
 * Range-based overload of *advance_min_if*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_min_if*
 * @return   The return value of the call to *advance_min_if*
 */
template <class... Args>
bool advance_min_if (mutable_range& r, Args&&... args)
{
    return advance_min_if(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Advances an iterator to the beginning of a position in a string that satsifies a scanning algorithm.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether the character was found
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          compound_scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_to_if_found (Iterator& first, Sentinel last,
                                    Expression e, Args&&... args)
{
    // Once ranges are standard, replace with range version of find_if
    auto copy = first;

    advance_while_not(copy, last, e, forward<Args>(args)...);

    if (copy == last)    return false;

    first = copy;
    return true;
}


template <input_iterator Iterator>
bool advance_to_if_found (Iterator& first, Iterator last,
                                    string_view literal)
{
    const auto searcher = boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).first;

    if (it == last)    return false;

    first = it;
    return true;
}


template <input_iterator Iterator>
bool advance_to_if_found (Iterator& first, Iterator last,
                                    char c)
{
    auto it = find(first, last, c);
    if (it == last)    return false;

    first = it;
    return true;
}


template <input_iterator Iterator>
bool advance_to_if_found (Iterator& first, Iterator last,
                                    char_predicate p)
{
    auto it = find_if(first, last, p);
    if (it == last)    return false;

    first = it;
    return true;
}


/**
 * Range-based overload of *advance_to_if_found*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_to_if_found*
 * @return   The return value of the call to *advance_to_if_found*
 */
template <class... Args>
bool advance_to_if_found (mutable_range& r, Args&&... args)
{
    return advance_to_if_found(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Advances an iterator to the end of a position in a string that satsifies a scanning algorithm.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *e*
 * @return   Whether the character was found
 */
template <input_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          class... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool advance_past_if_found (Iterator& first, Sentinel last,
                                      Expression e, Args... args)
{
    // Once ranges are standard, replace with range version of find_if
    auto copy = first;

    while (advance_if_not(copy, last, e, args...))
        if (copy == last)    return false;

    first = copy;
    advance_if(first, last, e, args...);

    return true;
}


template <input_iterator Iterator, class T>
bool advance_past_if_found (Iterator& first, Iterator last,
                                      string_view literal)
{
    const T searcher = boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).second;

    if (it == last)    return false;

    first = it;
    return true;
}


template <input_iterator Iterator>
bool advance_past_if_found (Iterator& first, Iterator last,
                                      char c)
{
    auto it = find(first, last, c);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


template <input_iterator Iterator>
bool advance_past_if_found (Iterator& first, Iterator last,
                                      char_predicate p)
{
    auto it = find_if(first, last, p);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


/**
 * Range-based overload of *advance_past_if_found*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_past_if_found*
 * @return   The return value of the call to *advance_past_if_found*
 */
template <class... Args>
bool advance_past_if_found (mutable_range& r, Args&&... args)
{
    return advance_past_if_found(r.begin(), r.end(), forward<Args>(args)...);
}









// ---------------------------------------------------------------------------------------------------------------------
// Additional character interfaces (move elsewhere?)
// ---------------------------------------------------------------------------------------------------------------------
templace <class T>
concept char_iterator = input_or_output_iterator<T> && same_as<char, iter_value_t<T>>;


struct char_sentinel
{
    constexpr bool operator== (char c)    { return c == '\0'; }
};


template <input_iterator I>
bool scan_impl (I& first, range literal)
{
    return scan_impl(first, char_sentinel{}, literal.begin(), literal.end());
}

template <input_iterator I1, input_iterator I2, sentinel_for<I2> S2>
bool scan_impl (I1& first1, I2 first2, S2 last2)
{
    return scan_impl(first1, char_sentinel{}, first2, last2);
}


// adding the length manually is acceptable until the library is heavily used (like for a char* scanner)
template <input_iterator I1, random_access_iterator I2>
bool scan_impl (I1& first1, I2 first2, int length)
{
    return scan_impl(first1, char_sentinel{}, first2, first2 + length);
}


template <random_access_iterator I, class... Args>
bool scan_impl (I& first1, int length, Args&&... args)
{
    return scan_impl(first1, first1 + length, forward<Args>(args)...);
}


template <input_iterator I1, sentinel_for<I1> S1, random_access_iterator I2>
bool scan_impl (I1& first1, S1 last1, I2 first2, int length)
{
    return scan_impl(first1, last1, first2, first2 + length);
}


// .. note::

//     **Rationale:**

//     Unfortunately, allowing implicit sentinels for both ranges introduces a syntactic ambiguity when three iterators are
//     passed in.

//         :expr:`scan(const char*, const char*, const char*)` could be either :expr:`scan(first1, last1, first2)` or
//         :expr:`scan(first1, first2, last2)`

//     If we arbitrarily choose one version, this signature becomes a special case, which leads to confusion. Since the
//     second range, if present, has known content, its length or sentinel is highly likely to be known as well. So the
//     first range was chosen as the better candidate for taking an implicit sentinel.






} // namespace exports
} // namespace internal

using namespace internal::exports;

} // namespace Pattern
