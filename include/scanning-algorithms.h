#ifndef SCANNING_ALGORITHMS
#define SCANNING_ALGORITHMS

#include <algorithm>
#include <string_view>
using std::string_view;


template <typename Pred>
concept bool char_predicate =
    // Callable<Pred> &&
    requires (Pred p, char c)
    {
        { p(c) } -> bool;
    };


// ------------------------
// Predicates
// ------------------------
template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last, char c)
{
    return (first < last && *first == c);
}


template <typename InputIt, typename Sentinel>
constexpr bool starts_with (InputIt first, Sentinel last, char_predicate p)
{
    return (first < last && p(*first));
}


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


template <typename Range, typename T>
constexpr bool starts_with (Range r, T t)
{
    return starts_with(r.begin(), r.end(), t);
}


// ------------------------
// Algorithms
// ------------------------
template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_if (InputIt& first, Sentinel last, T t)
{
    if (!starts_with(first, last, t))    return false;
    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_if (InputIt& first, Sentinel last, string_view literal)
{
    InputIt copy = first;
    if (!starts_with(&copy, last, literal))    return false;

    first = copy;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, char c)
{
    if (starts_with(first, last, c))    return false;
    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, char_predicate p)
{
    if (starts_with(first, last, p))    return false;
    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_if_not (InputIt& first, Sentinel last, string_view literal)
{
    if (starts_with(first, last, literal))    return false;
    ++first;
    return true;
}


// Returns true even on no match
template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_optionally (InputIt& first, Sentinel last, T t)
{
    advance_if(first, last, t);
    return true;
}


// Returns true even on no match, like a Kleene star
template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, char c)
{
    while (starts_with(first, last, c))    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, char_predicate p)
{
    while (starts_with(first, last, p))    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_while (InputIt& first, Sentinel last, string_view literal)
{
    while (advance_if(first, last, literal));
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_while_not (InputIt& first, Sentinel last, char c)
{
    while (!starts_with(first, last, c))    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_while_not (InputIt& first, Sentinel last, char_predicate p)
{
    while (!starts_with(first, last, p))    ++first;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_max_if (InputIt& first, Sentinel last, char c, size_t max = -1)
{
    last = first + std::min(last - first, max);
    while (advance_if(first, last, c));
    return true;
}


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


// Probably needs an optional return value
template <typename InputIt, typename Sentinel>
constexpr bool advance_min_if (InputIt& first, Sentinel last, char c, size_t min = 0)
{
    if (!advance_n_if(first, last, c, min))    return false;
    advance_while(first, last, c);
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_repeating (InputIt& first, Sentinel last, char c,
                                  size_t min = 0, size_t max = -1)
{
    if (max < min)    return false;

    if (!advance_n_if(first, last, c, min))    return false;
    advance_max_if(first, last, c, max - min);
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, char_predicate p)
{
    auto it = std::find_if(first, last, p);
    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_to_if_found (InputIt& first, Sentinel last, string_view literal)
{
    const T searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).first;

    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, char c)
{
    auto it = std::find(first, last, c);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


template <typename InputIt, typename Sentinel>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, char_predicate p)
{
    auto it = std::find_if(first, last, p);
    if (it == last)    return false;

    first = it + 1;
    return true;
}


template <typename InputIt, typename Sentinel, typename T>
constexpr bool advance_past_if_found (InputIt& first, Sentinel last, string_view literal)
{
    const T searcher = std::boyer_moore_searcher(literal.begin(), literal.end());
    auto it = searcher(first, last).second;

    if (it == last)    return false;

    first = it;
    return true;
}


template <typename InputIt, typename Sentinel, typename... Exp>
constexpr bool advance_if_any (InputIt& first, Sentinel last, Exp... e)
{
    return (... || e(first, last));
}


template <typename InputIt, typename Sentinel, typename... Exp>
constexpr bool advance_joined_if (InputIt& first, Sentinel last, Exp... e)
{
    InputIt copy = first;

    if (!(... && e(copy, last)))    return false;

    first = copy;
    return true;
}


#endif // WSCANNING
