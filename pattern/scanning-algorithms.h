/*
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Scanning Algorithms
 *
 * Algorithms for scanning sequences of elements.
 *
 */

#pragma once

#include <algorithm>       // std::ranges::mismatch
#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>     // std::is_reference_v
#include <utility>         // std::declval

#include "scanning-concepts.h"


namespace Pattern {


// =====================================================================================================================
// Concepts
// =====================================================================================================================
template <class R>
concept mutable_forward_range =
     std::ranges::forward_range<R> &&
     std::is_reference_v<decltype(begin(std::declval<R>()))>;


// =====================================================================================================================
// Scanning Functions
// =====================================================================================================================
struct scan_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::iter_value_t<I>>
     bool operator() (I& first, S last, E element) const
     {
          if (first == last || *first != element)     return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::ranges::range_value_t<R>>
     bool operator() (R&& r, E element) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r), std::move(element));
     }


     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool operator() (I1& first1, S1 last1, I2 first2, S2 last2) const
     {
          if constexpr (std::sized_sentinel_for<S1, I1> && std::sized_sentinel_for<S2, I2>)
          {
               if (std::ranges::distance(first1, last1) < std::ranges::distance(first2, last2))     return false;
          }

          auto ptrs = std::ranges::mismatch(first1, last1, first2, last2);

          if (ptrs.in2 != last2)    return false;

          first1 = ptrs.in1;
          return true;
     }


     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (R&& r, I first, S last) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r),
                            std::move(first), std::move(last));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (I& first, S last, R&& r) const
     {
          return operator()(first, std::move(last),
                            std::ranges::begin(r), std::ranges::end(r));
     }


     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool operator() (R1&& r1, R2&& r2) const
     {
          using std::begin;
          return operator()(begin(r1), std::ranges::end(r1),
                            std::ranges::begin(r2), std::ranges::end(r2));
     }


     // --------------------------------------------------
     // Character literal specialization
     // --------------------------------------------------
     template <class CharT, std::forward_iterator I, std::sentinel_for<I> S>
          requires std::equality_comparable_with<CharT, std::iter_value_t<I>>
     bool operator() (I& first, S last, const CharT* comparison) const
     {
          return operator()(first, std::move(last), std::string_view {comparison});
     }


     template <class CharT, mutable_forward_range R>
          requires std::equality_comparable_with<CharT, std::ranges::range_value_t<R>>
     bool operator() (R&& r, const CharT* comparison) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r),
                            std::string_view {comparison});
     }

 } // struct scan_t
scan;


struct scan_if_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool operator() (I& first, S last, P pred) const
     {
          if (first == last || !std::invoke(pred, *first))     return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::iterator_t<R>> P>
     bool operator() (R&& r, P pred) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r), std::move(pred));
     }

} // struct scan_if_t
scan_if;


struct scan_not_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::iter_value_t<I>>
     bool operator() (I& first, S last, E element) const
     {
          if (first == last || *first == element)     return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::ranges::range_value_t<R>>
     bool operator() (R&& r, E element) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r), std::move(element));
     }


     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool operator() (I1& first1, S1 last1, I2 first2, S2 last2) const
     {
          if constexpr (std::sized_sentinel_for<S1, I1> && std::sized_sentinel_for<S2, I2>)
          {
               if (std::ranges::distance(first1, last1) < std::ranges::distance(first2, last2))
               {
                    ++first1;
                    return true;
               }
          }

          auto ptrs = std::ranges::mismatch(first1, last1, first2, last2);

          if (ptrs.in2 == last2)     return false;

          ++first1;
          return true;
     }


     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (R&& r, I first, S last) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r),
                            std::move(first), std::move(last));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (I& first, S last, R&& r) const
     {
          return operator()(first, std::move(last),
                            std::ranges::begin(r), std::ranges::end(r));
     }


     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool operator() (R1&& r1, R2&& r2) const
     {
          using std::begin;
          return operator()(begin(r1), std::ranges::end(r1),
                            std::ranges::begin(r2), std::ranges::end(r2));
     }


     // --------------------------------------------------
     // Character literal specialization
     // --------------------------------------------------
     template <class CharT, std::forward_iterator I, std::sentinel_for<I> S>
          requires std::equality_comparable_with<CharT, std::iter_value_t<I>>
     bool operator() (I& first, S last, const CharT* comparison) const
     {
          return operator()(first, std::move(last), std::string_view {comparison});
     }


     template <class CharT, mutable_forward_range R>
          requires std::equality_comparable_with<CharT, std::ranges::range_value_t<R>>
     bool operator() (R&& r, const CharT* comparison) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r),
                            std::string_view {comparison});
     }

} // struct scan_not_t
scan_not;


struct scan_if_not_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool operator() (I& first, S last, P pred) const
     {
          if (first == last || std::invoke(pred, *first))     return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::iterator_t<R>> P>
     bool operator() (R&& r, P pred) const
     {
          using std::begin;
          return operator()(begin(r), std::ranges::end(r), std::move(pred));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S,
               boolean_invocable<I, S> F>
     bool operator() (I& first, S last, F f) const
     {
          I copy = first;
          if (std::invoke(f, copy, last))     return false;

          ++first;
          return true;
     }


     template <mutable_forward_range R, boolean_invocable<R> F>
     bool operator() (R&& r, F f) const
     {
          auto copy = r;
          if (std::invoke(f, copy))     return false;

          using std::begin;
          auto& first = begin(r);

          ++first;
          return true;
     }

} // struct scan_if_not_t
scan_if_not;


} // namespace Pattern
