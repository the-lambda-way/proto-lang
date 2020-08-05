/**
 * Scanning Algorithms
 *
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Algorithms for scanning sequences of elements.
 *
 */

#pragma once

#include <algorithm>       // std::ranges::mismatch
#include <concepts>
#include <iterator>
#include <functional>      // std::identity
#include <ranges>
#include <type_traits>     // std::is_reference_v

#include "scanning-concepts.h"


namespace Pattern {


// =====================================================================================================================
// Concepts
// =====================================================================================================================
template <class F, class I, class S>
concept scanning_algorithm = std::forward_iterator<I> && std::sentinel_for<S, I> && boolean_invocable<F, I&, S>;


template <class R>
concept mutable_forward_range = std::ranges::forward_range<R> && std::is_reference_v<std::ranges::iterator_t<R>>;


// =====================================================================================================================
// Scanning Functions
// =====================================================================================================================
struct scan_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::equality_comparable_with<std::iter_value_t<I>> E>
     bool operator() (I& first, S last, E element)
     {
          if (first == last || *first != element)    return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::equality_comparable_with<std::ranges::iterator_t<R>> E>
     bool operator() (R&& r, E&& element)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r), std::forward<E>(element));
     }


     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool operator() (I1& first1, S1 last1, I2 first2, S2 last2)
     {
          if constexpr (std::sized_sentinel_for<S1, I1> && std::sized_sentinel_for<S2, I2>)
          {
               if (std::ranges::distance(first1, last1) < std::ranges::distance(first2, last2))    return false;
          }

          auto ptrs = std::ranges::mismatch(first1, last1, first2, last2);

          if (ptrs.in2 != last2)    return false;

          first1 = ptrs.in1;
          return true;
     }


     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (R&& r, I&& first, S&& last)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r),
                            std::forward<I>(first), std::forward<S>(last));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (I&& first, S&& last, R&& r)
     {
          return operator()(std::forward<I>(first), std::forward<S>(last),
                            std::ranges::begin(r), std::ranges::end(r));
     }


     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool operator() (R1&& r1, R2&& r2)
     {
          return operator()(std::ranges::begin(r1), std::ranges::end(r1),
                            std::ranges::begin(r2), std::ranges::end(r2));
     }
} // struct scan_t
scan;


struct scan_if_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool operator() (I& first, S last, P pred)
     {
          if (first == last || !std::invoke(pred, *first))    return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::range_value_t<R>> P>
     bool operator() (R&& r, P&& pred)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r), std::forward<P>(pred));
     }
} // struct scan_if_t
scan_if;


struct scan_not_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::equality_comparable_with<std::iter_value_t<I>> E>
     bool operator() (I& first, S last, E element)
     {
          if (first == last || *first == element)    return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::equality_comparable_with<std::ranges::iterator_t<R>> E>
     bool operator() (R&& r, E&& element)
     {
          return scan_if_not(std::ranges::begin(r), std::ranges::end(r), std::forward<E>(element));
     }


     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool operator() (I1& first1, S1 last1, I2 first2, S2 last2)
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

          if (ptrs.in2 == last2)    return false;

          ++first1;
          return true;
     }


     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (R&& r, I&& first, S&& last)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r),
                            std::forward<I>(first), std::forward<S>(last));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool operator() (I&& first, S&& last, R&& r)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r),
                            std::foward<I>(first), std::foward<S>(last));
     }


     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool operator() (R1&& r1, R2&& r2)
     {
          return operator()(std::ranges::begin(r1), std::ranges::end(r1),
                            std::ranges::begin(r2), std::ranges::end(r2));
     }
} // struct scan_not_t
scan_not;


struct scan_if_not_t
{
     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool operator() (I& first, S last, P pred)
     {
          if (first == last || std::invoke(pred, *first))    return false;
          ++first;
          return true;
     }


     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::range_value_t<R>> P>
     bool operator() (R&& r, P&& pred)
     {
          return operator()(std::forward<P>(pred), std::ranges::begin(r), std::ranges::end(r));
     }


     template <std::forward_iterator I, std::sentinel_for<I> S,
               scanning_algorithm<I, S> F>
     bool operator() (I& first, S last, F f)
     {
          I copy = first;
          if (std::invoke(f, copy, last))    return false;

          ++first;
          return true;
     }


     template <mutable_forward_range R,
               scanning_algorithm<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> F>
     bool operator() (R&& r, F&& f)
     {
          return operator()(std::ranges::begin(r), std::ranges::end(r), std::forward<F>(f));
     }
} // struct scan_if_not_t
scan_if_not;


} // namespace Pattern
