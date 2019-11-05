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
 * A set of algorithms for scanning character-based sources.
 */


#ifndef SCANNING_ALGORITHMS
#define SCANNING_ALGORITHMS

#include <algorithm>      // std::find
#include <string_view>
#include <type_traits>    // std::remove_reference, std::is_same
#include "concepts-kludge.h"

using std::string_view;


// ---------------------------------------------------------------------------------------------------------------------
//  Concepts
// ---------------------------------------------------------------------------------------------------------------------
/**
 * A character predicate receives a single character and returns true if it matches a criteria.
 *
 * @param   char   The first character of a string
 */
template <typename Function, typename... Args>
concept bool char_predicate =
    requires (Function f, char c, Args... args)
    {
        { f(c, args...) } -> bool;
    };


/**
 * An atomic scannable expression is an expression that can be compared to a single character.
 */
template <typename T, typename... Args>
concept bool atomic_scannable_expression =
    std::is_same_v<T, char> ||
    char_predicate<T, Args...>;


/**
 * A scanning algorithm receives a sequence of characters and returns true if it matches a criteria.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    args...   Arguments passed to the algorithm
 */
template <typename Function,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args>
concept bool scanning_algorithm =
    requires (Function f, Iterator& first, Sentinel last, Args... args)
    {
        { f(first, last, args...) } -> bool;
        { first.begin() || true } -> bool;    // first must not be a range, this ad-hoc solution should be replaced once Iterator concepts are available
    };


/**
 * Concept for a compound, scannable expression
 */
template <typename T,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args>
concept bool compound_scannable_expression =
    std::is_same_v<T, string_view> ||
    scanning_algorithm<T, Iterator, Sentinel, Args...>;


/**
 * Concept for a scannable expression.
 */
template <typename T,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args>
concept bool scannable_expression =
    atomic_scannable_expression<T, Args...> ||
    compound_scannable_expression<T, Iterator, Sentinel, Args...>;


// ---------------------------------------------------------------------------------------------------------------------
// Predicates
// ---------------------------------------------------------------------------------------------------------------------
/**
 * Bounded check, whether a string begins with a certain character.
 *
 * @param    first   Iterator to the start of a string
 * @param    last    Sentinel to the end of the string
 * @param    c       Character to compare equal with
 * @return   Whether the character compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last,
                            char c)
{
    return (first < last && *first == c);
}


/**
 * Bounded check, whether a string begins with a certain string.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    literal   String to compare equal with
 * @return   Whether *literal* compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last,
                            string_view literal)
{
    if constexpr (!starts_with(first, last, literal.front()))    return false;
    if constexpr (last - first > literal.length())               return false;

    ++first;

    for (auto i = literal.begin() + 1;    i != literal.end();    ++i, ++first)
        if (*first != *i)    return false;

    return true;
}


/**
 * @name scan_with
 * Delegates the appropriate function call for various scannable expressions
 */
///@{

/**
 * Bounded check, whether a string begins with a certain character.
 *
 * @param    first   Iterator to the start of a string
 * @param    last    Sentinel to the end of the string
 * @param    c       Character to compare equal with
 * @return   Whether the character compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool scan_with (InputIt first, Sentinel last,
                          char c)
{
    return starts_with(first, last, c);
}


/**
 * Bounded check, whether the first character of a string satisfies a predicate.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    p         Predicate to test the string with
 * @param    args...   Arguments passed to *p*
 * @return   Whether the predicate returned true
 */
template <typename InputIt, typename Sentinel,
          typename... Args>
constexpr bool scan_with (InputIt first, Sentinel last,
                          char_predicate<Args...> p, Args&&... args)
{
    return (first < last && p(*first, forward<Args>(args)...));
}


/**
 * Bounded check, whether a string begins with a certain string.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    literal   String to compare equal with
 * @return   Whether *literal* compared equal
 */
// Compound algorithms take the begin iterator by reference, so the iterator can advance during the predicate check.
template <typename InputIt, typename Sentinel>
constexpr bool scan_with (InputIt& first, Sentinel last,
                          string_view literal)
{
    return starts_with(&first, last, literal);
}


/**
 * Checks whether a sequence at the beginning of a string satisfies a predicate.
 *
 * @param    first     Iterator to the start of a string
 * @param    last      Sentinel to the end of the string
 * @param    e         Expression to test the string with
 * @param    args...   Arguments passed to *f*
 * @return   Whether the scanning algorithm returned true
 */
template <typename InputIt, typename Sentinel,
          typename... Args, scanning_algorithm<InputIt, Sentinel, Args...> Func>
constexpr bool scan_with (InputIt& first, Sentinel last,
                          Func f, Args&&... args)
{
    return f(first, last, forward<Args>(args)...);
}
///@}


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
template <typename InputIt, typename Sentinel,
          typename... Args, atomic_scannable_expression<Args...> Exp>
constexpr bool advance_if (InputIt& first, Sentinel last,
                           Exp e, Args... args)
{
    if (!scan_with(first, last, e, forward<Args>(args)...))    return false;
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
template <typename InputIt, typename Sentinel,
          typename... Args, compound_scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_if (InputIt& first, Sentinel last,
                           Exp e, Args&&... args)
{
    InputIt copy = first;

    if (!scan_with(copy, last, e, forward<Args>(args)...))    return false;

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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_if_not (InputIt& first, Sentinel last,
                               Exp e, Args&&... args)
{
    if (scan_with(first, last, e, forward<Args>(args)...))    return false;
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_optionally (InputIt& first, Sentinel last,
                                   Exp e, Args&&... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_while (InputIt& first, Sentinel last,
                              Exp e, Args... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, atomic_scannable_expression<Args...> Exp>
constexpr bool advance_max_if (InputIt& first, Sentinel last,
                               Exp e, Args&&... args,
                               size_t max = -1)
{
    // No algorithm can advance more times than the number of characters remaining (in theory)
    // *Sentinel* might not be convertible to *InputIt*, so a few extra steps are required
    size_t diff = last - first;
    last = last - diff + std::min(diff, max);

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
template <typename InputIt, typename Sentinel,
          typename... Args, compound_scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_max_if (InputIt& first, Sentinel last,
                               Exp e, Args... args,
                               size_t max = -1)
{
    while (max-- && advance_if(first, last, e, args...));
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_n_if (InputIt& first, Sentinel last,
                             Exp e, Args... args,
                             size_t n)
{
    if (first - last < n)    return false;
    if (n == 0)              return true;
    if (n == 1)              return advance_if(first, last, e, args...);

    InputIt copy = first;

    while (n--)
        if (!advance_if(copy, last, e, args...))    return false;

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
template <typename InputIt, typename Sentinel,
          scannable_expression<InputIt, Sentinel>... Exp>
constexpr bool advance_if_any (InputIt& first, Sentinel last,
                               Exp... e)
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
template <typename InputIt, typename Sentinel,
          scannable_expression<InputIt, Sentinel>... Exp>
constexpr bool advance_join_if (InputIt& first, Sentinel last,
                                Exp... e)
{
    InputIt copy = first;

    if (!(... && advance_if(copy, last, e)))    return false;

    first = copy;
    return true;
}


// ---------------------------------------------------------------------------------------------------------------------
// Ranged Versions
// ---------------------------------------------------------------------------------------------------------------------
/**
 * A mutable range supplies a reference to its begin iterator.
 */
template <typename T>
concept bool mutable_range =
    requires (T t)
    {
        { t.begin() } -> std::remove_reference_t<decltype(t.begin())>&;
        { t.end()   }
    };


/**
 * Range-based overload of *starts_with*
 *
 * @param    r   Range representing a string
 * @param    t   Templated argument to *starts_with*
 * @return   The return value of the call to *starts_with*
 */
template <typename Range, typename T>
constexpr bool starts_with (Range r, T t)
{
    return starts_with(r.begin(), r.end(), t);
}


/**
 * Range-based overload of *scan_with*
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *scan_with*
 * @return   The return value of the call to *scan_with*
 */
template <typename... Args>
constexpr bool scan_with (mutable_range& r, Args... args)
{
    return scan_with(r.begin(), r.end(), forward<Args>(args)...);
}


/**
 * Range-based overload of *advance_if*
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_if*
 * @return   The return value of the call to *advance_if*
 */
template <typename... Args>
constexpr bool advance_if (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_if_not (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_optionally (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_while (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_max_if (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_n_if (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_if_any (mutable_range& r, Args&&... args)
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
template <typename... Args>
constexpr bool advance_join_if (mutable_range& r, Args&&... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_while_not (InputIt& first, Sentinel last,
                                  Exp e, Args... args)
{
    while (advance_if_not(first, last, e, args...));
    return true;
}


/**
 * Range-based overload of *advance_while_not*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_while_not*
 * @return   The return value of the call to *advance_while_not*
 */
template <typename... Args>
constexpr bool advance_while_not (mutable_range& r, Args&&... args)
{
    return advance_while_not(r.begin(), r.end(), forward<Args>(args)...);
}


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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_repeating (InputIt& first, Sentinel last,
                                  Exp e, Args... args,
                                  size_t min = 0, size_t max = -1)
{
    if (max < min)    return false;

    if (!advance_n_if(first, last, e, min, args...))    return false;
    advance_max_if(first, last, e, max - min, args...);
    return true;
}


/**
 * Range-based overload of *advance_repeating*.
 *
 * @param    r         Mutable range representing a string
 * @param    args...   Arguments forwarded to *advance_repeating*
 * @return   The return value of the call to *advance_repeating*
 */
template <typename... Args>
constexpr bool advance_repeating (mutable_range& r, Args&&... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_min_if (InputIt& first, Sentinel last,
                               Exp e, Args... args,
                               size_t min = 0)
{
    if (!advance_n_if(first, last, e, min, args...))    return false;
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
template <typename... Args>
constexpr bool advance_min_if (mutable_range& r, Args&&... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, compound_scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last,
                                    Exp e, Args&&... args)
{
    // Once ranges are standard, replace with range version of std::find_if
    auto copy = first;

    advance_while_not(copy, last, e, forward<Args>(args)...);

    if (copy == last)    return false;

    first = copy;
    return true;
}


template <typename InputIt>
constexpr bool advance_to_if_found (InputIt& first, InputIt last,
                                    string_view literal)
{
    const auto searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).first;

    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt>
constexpr bool advance_to_if_found (InputIt& first, InputIt last,
                                    char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt>
constexpr bool advance_to_if_found (InputIt& first, InputIt last,
                                    char_predicate p)
{
    auto it = std::find_if(first, last, p);
    if (it == last)    return false;

    first = it;
    return true;
}


/**
 * Range-based overload of *advance_to_if_found*.
 *
 * @param    r         Mutable range representing a character sequence
 * @param    args...   Arguments forwarded to *advance_to_if_found*
 * @return   The return value of the call to *advance_to_if_found*
 */
template <typename... Args>
constexpr bool advance_to_if_found (mutable_range& r, Args&&... args)
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
template <typename InputIt, typename Sentinel,
          typename... Args, scannable_expression<InputIt, Sentinel, Args...> Exp>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last,
                                      Exp e, Args... args)
{
    // Once ranges are standard, replace with range version of std::find_if
    auto copy = first;

    while (advance_if_not(copy, last, e, args...))
        if (copy == last)    return false;

    first = copy;
    advance_if(first, last, e, args...);

    return true;
}


template <typename InputIt, typename T>
constexpr bool advance_past_if_found (InputIt& first, InputIt last,
                                      string_view literal)
{
    const T searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).second;

    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt>
constexpr bool advance_past_if_found (InputIt& first, InputIt last,
                                      char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


template <typename InputIt>
constexpr bool advance_past_if_found (InputIt& first, InputIt last,
                                      char_predicate p)
{
    auto it = std::find_if(first, last, p);
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
template <typename... Args>
constexpr bool advance_past_if_found (mutable_range& r, Args&&... args)
{
    return advance_past_if_found(r.begin(), r.end(), forward<Args>(args)...);
}


#endif // SCANNING_ALGORITHMS
