/**
 * Mike Castillo
 *
 * Copyright (c) 2020 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Concepts useful for scanning applications.
 */

#pragma once

#include <concepts>
#include <ranges>


namespace Pattern {

// As of 2020-07-27, the concept boolean_testable remains exposition only. It's useful enough to implement in the
// meantime.
// Implementation based on https://en.cppreference.com/w/cpp/concepts/boolean-testable
template <class B>
concept boolean_testable =
     std::convertible_to<B, bool> &&
     requires (B&& b) {
          { !std::forward<B>(b) } -> std::convertible_to<bool>;
     };


// Unlike a predicate, a boolean_invocable is not equality preserving. Thus, a callable type F is allowed to modify the
// arguments it receives.
template <class F, class... Args>
concept boolean_invocable = std::invocable<F, Args...> &&
                            boolean_testable<std::invoke_result_t<F, Args...>>;


template <class I1, class I2, class P1 = std::identity, class P2 = std::identity>
concept indirectly_equality_comparable = std::indirectly_comparable<I1, I2, std::ranges::equal_to, P1, P2>;


template <class R1, class R2, class P1 = std::identity, class P2 = std::identity>
concept range_equality_comparable =
     std::ranges::range<R1> && std::ranges::range<R2> &&
     indirectly_equality_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>, P1, P2>;


} // namespace Pattern

