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
 *
 * This library is based on the semantics of context-free grammars.
 */

// TODO: Go through standard CFG elements and check for missing


#ifndef SCANNING_ALGORITHMS
#define SCANNING_ALGORITHMS

#include <algorithm>      // std::find
#include <string_view>

using std::string_view;


// ---------------------------------------------------------------------------------------------------------------------
//  Concepts
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Concept for a character predicate.
 *
 * @param   char   Input provided from a character sequence
 * @param   bool   Whether the predicate matched
 */
template <typename Pred>
concept bool char_predicate =
    // Callable<Pred> &&
    requires (Pred p, char c)
    {
        { p(c) } -> bool;
    };


// ---------------------------------------------------------------------------------------------------------------------
// Predicates
// ---------------------------------------------------------------------------------------------------------------------
/**
 * Bounded check, whether a character sequence begins with a certain character.
 *
 * @param    first   Iterator to the start of the sequence
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @return   Whether the character compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last, char c)
{
    return (first < last && *first == c);
}


/**
 * Bounded check, whether the beginning of character sequence satisfies a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Whether the predicate returned true
 */
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last, char_predicate p)
{
    return (first < last && p(*first));
}


/**
 * Bounded check, whether a character sequence begins with a string of characters.
 *
 * @param    first     Iterator to the start of the sequence
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Whether *literal* compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last, string_view literal)
{
    if constexpr (!starts_with(first, last, literal.front()))    return false;
    if constexpr (last - first > literal.length())               return false;

    ++first;

    for (auto i = literal.begin() + 1;    i != literal.end();    ++i, ++first)
        if (*first != *i)    return false;

    return true;
}


/**
 * Range-based overload of *starts_with*
 *
 * @param    r   Range representing a character sequence
 * @param    t   Templated argument to *starts_with*
 * @return   The return value of the call to *starts_with*
 */
template <typename Range, typename T>
constexpr bool starts_with (Range r, T t)
{
    return starts_with(r.begin(), r.end(), t);
}


// ---------------------------------------------------------------------------------------------------------------------
// Algorithms
// ---------------------------------------------------------------------------------------------------------------------
/**
 * Advances an iterator if a character sequence begins with a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @return   Whether the character compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if (InputIt& first, Sentinel last, char c)
{
    if (!starts_with(first, last, c))    return false;
    ++first;
    return true;
}


/**
 * Advances an iterator if the beginning of character sequence satisfies a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Whether the predicate returned true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if (InputIt& first, Sentinel last, char_predicate p)
{
    if (!starts_with(first, last, p))    return false;
    ++first;
    return true;
}


/**
 * Advances an iterator if a character sequence begins with a string of characters.
 *
 * @param    first     Iterator to the start of the sequence, to be advanced
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Whether literal compared equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if (InputIt& first, Sentinel last, string_view literal)
{
    InputIt copy = first;
    if (!starts_with(&copy, last, literal))    return false;

    first = copy;
    return true;
}


/**
 * Advances an iterator if a character sequence doesn't begin with a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @return   Whether the character compared not equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, char c)
{
    if (starts_with(first, last, c))    return false;
    ++first;
    return true;
}


/**
 * Advances an iterator if the beginning of character sequence doesn't satisfy a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Whether the predicate returned false
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, char_predicate p)
{
    if (starts_with(first, last, p))    return false;
    ++first;
    return true;
}


/**
 * Advances an iterator once if a character sequence doesn't begin with a string of characters.
 *
 * @param    first     Iterator to the start of the sequence, to be advanced
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Whether literal compared not equal
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, string_view literal)
{
    if (starts_with(first, last, literal))    return false;
    ++first;
    return true;
}


/**
 * Calls *advance_if* with an argument, then returns true regardless of the result.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    t       Template argument to *advance_if*
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_optionally (InputIt& first, Sentinel last, T t)
{
    advance_if(first, last, t);
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, char c)
{
    while (starts_with(first, last, c))    ++first;
    return true;
}


/**
 * Advances an iterator while the beginning of character sequence satisfies a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, char_predicate p)
{
    while (starts_with(first, last, p))    ++first;
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a string of characters.
 *
 * @param    first     Iterator to the start of the sequence, to be advanced
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, string_view literal)
{
    while (advance_if(first, last, literal));
    return true;
}


/**
 * Advances an iterator if a character sequence begins with a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_while_not (InputIt& first, Sentinel last, char c)
{
    while (!starts_with(first, last, c))    ++first;
    return true;
}


/**
 * Advances an iterator while the beginning of character sequence doesn't satisfy a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_while_not (InputIt& first, Sentinel last, char_predicate p)
{
    while (!starts_with(first, last, p))    ++first;
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a certain character, up to a maximum amount.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @param    max     Maximum number of times to repeat, -1 to match until fail
 * @return   Always returns true
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_max_if (InputIt& first, Sentinel last, char c, size_t max = -1)
{
    last = first + std::min(last - first, max);
    while (advance_if(first, last, c));
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a certain character, exactly n times.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @param    n       Number of times to repeat
 * @return   Whether n repeats were successful
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_n_if (InputIt& first, Sentinel last, char c, size_t n)
{
    if (first - last < n)    return false;
    if (n == 0)              return true;
    if (n == 1)              return advance_if(first, last, c);

    InputIt copy = first;

    while (copy != first + n)
        if (*copy++ != c)    return false;

    first = copy;
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a certain character, a minimum number of times.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @param    min     Minimum number of times to repeat
 * @return   Whether at least *min* repeats were successful
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_min_if (InputIt& first, Sentinel last, char c, size_t min = 0)
{
    if (!advance_n_if(first, last, c, min))    return false;
    advance_while(first, last, c);
    return true;
}


/**
 * Advances an iterator while a character sequence begins with a certain character, between min and max times.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to compare equal with
 * @param    min     Minimum number of times to repeat
 * @param    max     Maximum number of times to repeat, -1 to match until fail
 * @return   Whether at least *min* repeats were successful
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_repeating (InputIt& first, Sentinel last, char c,
                                  size_t min = 0, size_t max = -1)
{
    if (max < min)    return false;

    if (!advance_n_if(first, last, c, min))    return false;
    advance_max_if(first, last, c, max - min);
    return true;
}


/**
 * Advances an iterator to the next character in a sequence that matches a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to find
 * @return   Whether the character was found
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it;
    return true;
}


/**
 * Advances an iterator to the next character in a sequence that satisfies a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Whether a character was found that satisfies the predicate
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, char_predicate p)
{
    auto it = std::find_if(first, last, p);
    if (it == last)    return false;

    first = it;
    return true;
}


/**
 * Advances an iterator to the beginning of a position of string of characters, if found within a character sequence.
 *
 * @param    first     Iterator to the start of the sequence, to be advanced
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Whether literal was found
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, string_view literal)
{
    const auto searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).first;

    if (it == last)    return false;

    first = it;
    return true;
}

/**
 * Advances an iterator after the next character in a sequence that matches a certain character.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    c       Character to find
 * @return   Whether the character was found
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


/**
 * Advances an iterator after the next character in a sequence that satisfies a unary predicate.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    p       Predicate to test the sequence with
 * @return   Whether a character was found that satisfies the predicate
 */
template <typename InputIt, typename Sentinel>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, char_predicate p)
{
    auto it = std::find_if(first, last, p);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


/**
 * Advances an iterator past a string of characters, if found within a character sequence.
 *
 * @param    first     Iterator to the start of the sequence, to be advanced
 * @param    last      Sentinel to the end of the sequence
 * @param    literal   Character sequence to compare equal with
 * @return   Whether literal was found
 */
template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, string_view literal)
{
    const T searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).second;

    if (it == last)    return false;

    first = it;
    return true;
}


/**
 * Calls a series of scanning algorithms, returning true if any of them are satisfied.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    e...    Expressions to apply the sequence to
 * @return   Whether any of the expressions returned true
 */
template <typename InputIt, typename Sentinel, typename... Exp>
constexpr bool advance_if_any (InputIt& first, Sentinel last, Exp... e)
{
    return (... || e(first, last));
}


/**
 * Calls a series of scanning algorithms in order, returning true if all of them are satisfied.
 *
 * @param    first   Iterator to the start of the sequence, to be advanced
 * @param    last    Sentinel to the end of the sequence
 * @param    e...    Expressions to apply the sequence to
 * @return   Whether all of the expressions returned true
 */
template <typename InputIt, typename Sentinel, typename... Exp>
constexpr bool advance_joined_if (InputIt& first, Sentinel last, Exp... e)
{
    InputIt copy = first;

    if (!(... && e(copy, last)))    return false;

    first = copy;
    return true;
}


#endif // SCANNING_ALGORITHMS
