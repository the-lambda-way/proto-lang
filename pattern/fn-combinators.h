/**
 * Function Combinators
 *
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Facilities for declaratively composing functions.
 *
 */

// Compare to https://github.com/rollbear/lift

// TODO: A well-designed Curry function object would allow us to combine namespaces fn and fo without loss of
// usability.


#pragma once

#include <concepts>
#include <cstddef>         // std::size_t
#include <functional>      // std::invoke, std::bind_front
#include <tuple>           // any, all
#include <type_traits>     // std::invoke_result_t
#include <utility>         // std::forward

#include "scanning-concepts.h"


namespace Pattern {
namespace fn {

// =====================================================================================================================
// Algorithms
// =====================================================================================================================
auto identity =
[]
<class... Args, boolean_invocable<Args...> F>
(F&& f, Args&&... args) -> bool
{
     return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};


auto negate =
[]
<class... Args, boolean_invocable<Args...> F>
(F&& f, Args&&... args) -> bool
{
     return !std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};


auto optional =
[]
<class... Args, boolean_invocable<Args...> F>
(F&& f, Args&&... args) -> bool
{
     std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
     return true;
};


auto at_most =
[]
<class... Args, boolean_invocable<Args...> F>
(size_t n, F&& f, Args&&... args) -> bool
{
     ++n;
     while (--n && std::invoke(f, args...));
     return true;
};


auto n_times =
[]
<class... Args, boolean_invocable<Args...> F>
(size_t n, F&& f, Args&&... args) -> bool
{
     ++n;
     while (--n)
         if (!std::invoke(f, args...))    return false;
     return true;
};


auto repeat =
[]
<class... Args, boolean_invocable<Args...> F>
(size_t min, size_t max, F&& f, Args&&... args) -> bool
{
     if (max < min)    return false;

     return n_times(min, f, args...) && at_most(max - min, f, args...);
};


auto many =
[]
<class... Args, boolean_invocable<Args...> F>
(F&& f, Args&&... args) -> bool
{
     while (std::invoke(f, args...));
     return true;
};


auto at_least =
[]
<class... Args, boolean_invocable<Args...> F>
(size_t n, F&& f, Args&&... args) -> bool
{
     return n_times(n, f, args...) && many(f, args...);
};


auto some =
[]
<class... Args, boolean_invocable<Args...> F>
(F&& f, Args&&... args) -> bool
{
     return at_least(1, std::forward<F>(f), std::forward<Args>(args)...);
};


struct any_t
{
     bool operator () (boolean_invocable auto&&... f)
     {
          return (... || std::invoke(std::forward<decltype(f)>(f)));
     }

     template <class... Args, boolean_invocable<Args...>... F>
     bool operator () (F&&... f, std::tuple<Args...>&& args)
     {
          return (... || std::apply(std::forward<decltype(f)>(f), args));
     };
}
any;


struct all_t
{
     bool operator () (boolean_invocable auto&&... f)
     {
          return (... && std::invoke(std::forward<decltype(f)>(f)));
     }

     template <class... Args, boolean_invocable<Args...>... F>
     bool operator () (F&&... f, std::tuple<Args...>&& args)
     {
          return (... && std::apply(std::forward<decltype(f)>(f), args));
     };
}
all;


// =====================================================================================================================
// Utilities
// =====================================================================================================================
// Modifcation of std::bind_front
template <typename F, typename... BoundArgs>
struct bind_back_t
{
     static_assert(std::is_move_constructible_v<F>);
     static_assert((std::is_move_constructible_v<BoundArgs> && ...));

     // First parameter is to ensure this constructor is never used instead of the copy/move constructor.
     template<typename Fn, typename... Args>
     explicit constexpr bind_back_t (int, Fn&& fn, Args&&... args)
          noexcept(std::conjunction_v<std::is_nothrow_constructible<F, Fn>,
                                      std::is_nothrow_constructible<BoundArgs, Args>...>)
          : f {std::forward<Fn>(fn)}, bound_args {std::forward<Args>(args)...}
     {
          static_assert(sizeof...(Args) == sizeof...(BoundArgs));
     }

     bind_back_t (const bind_back_t&)            = default;
     bind_back_t (bind_back_t&&)                 = default;
     bind_back_t& operator= (const bind_back_t&) = default;
     bind_back_t& operator= (bind_back_t&&)      = default;
     ~bind_back_t ()                             = default;

     template<typename... CallArgs>
     constexpr std::invoke_result_t<F&, CallArgs..., BoundArgs&...>
     operator() (CallArgs&&... call_args) &
          noexcept(std::is_nothrow_invocable_v<F&, CallArgs..., BoundArgs&...>)
     {
          return S_call(*this, BoundIndices(), std::forward<CallArgs>(call_args)...);
     }

     template<typename... CallArgs>
     constexpr std::invoke_result_t<const F&, CallArgs..., const BoundArgs&...>
     operator() (CallArgs&&... call_args) const &
          noexcept(std::is_nothrow_invocable_v<const F&, CallArgs..., const BoundArgs&...>)
     {
          return S_call(*this, BoundIndices(), std::forward<CallArgs>(call_args)...);
     }

     template<typename... CallArgs>
     constexpr std::invoke_result_t<F, CallArgs..., BoundArgs...>
     operator() (CallArgs&&... call_args) &&
          noexcept(std::is_nothrow_invocable_v<F, CallArgs..., BoundArgs...>)
     {
          return S_call(std::move(*this), BoundIndices(), std::forward<CallArgs>(call_args)...);
     }

     template<typename... CallArgs>
     constexpr std::invoke_result_t<const F, CallArgs..., const BoundArgs...>
     operator() (CallArgs&&... call_args) const &&
          noexcept(std::is_nothrow_invocable_v<const F, CallArgs..., const BoundArgs...>)
     {
          return S_call(std::move(*this), BoundIndices(), std::forward<CallArgs>(call_args)...);
     }

private:
     F f;
     std::tuple<BoundArgs...> bound_args;

     using BoundIndices = std::index_sequence_for<BoundArgs...>;

     template<typename T, size_t... Ind, typename... CallArgs>
     static constexpr decltype(auto)
     S_call (T&& g, std::index_sequence<Ind...>, CallArgs&&... call_args)
     {
          return std::invoke(std::forward<T>(g).f,
                             std::forward<CallArgs>(call_args)...,
                             std::get<Ind>(std::forward<T>(g).bound_args)...);
     }
}; // struct bind_back_t


} // namespace fn


template <typename F, typename... Args>
using _bind_back_t = fn::bind_back_t<std::decay_t<F>, std::decay_t<Args>...>;


namespace fn {

template <typename F, typename... Args>
constexpr _bind_back_t<F, Args...>
bind_back (F&& f, Args&&... args)
    noexcept(std::is_nothrow_constructible_v<int, _bind_back_t<F, Args...>, F, Args...>)
{
    return _bind_back_t<F, Args...>(0, std::forward<F>(f), std::forward<Args>(args)...);
}


} // namespace fn


namespace fo {

// =====================================================================================================================
// Combinators
// =====================================================================================================================
auto identity = [] (auto&& f)                { return std::bind_front(fn::identity,   std::forward<decltype(f)>(f)); };
auto negate   = [] (auto&& f)                { return std::bind_front(fn::negate,     std::forward<decltype(f)>(f)); };
auto optional = [] (auto&& f)                { return std::bind_front(fn::optional,   std::forward<decltype(f)>(f)); };
auto at_most  = [] (std::size_t n, auto&& f) { return std::bind_front(fn::at_most, n, std::forward<decltype(f)>(f)); };
auto n_times  = [] (std::size_t n, auto&& f) { return std::bind_front(fn::n_times, n, std::forward<decltype(f)>(f)); };
auto many     = [] (auto&& f)                { return std::bind_front(fn::many,       std::forward<decltype(f)>(f)); };

auto repeat = [] (size_t min, size_t max, auto&& f)
{
     return std::bind_front(fn::repeat, min, max, std::forward<decltype(f)>(f));
};

auto at_least = [] (std::size_t n, auto&& f) { return std::bind_front(fn::at_least, n, std::forward<decltype(f)>(f)); };
auto some     = [] (auto&& f)                { return std::bind_front(fn::some,        std::forward<decltype(f)>(f)); };

auto any = [] (auto&&... f)
{
     return
          [f...]
          <class... CallArgs>
               requires (... && boolean_invocable<decltype(f), CallArgs...>)
          (CallArgs&&... call_args) -> bool
          {
               return (... || std::invoke(f, call_args...));
          };
};


auto all = [] (auto&&... f)
{
     return
          [f...]
          <class... CallArgs>
               requires (... && boolean_invocable<decltype(f), CallArgs...>)
          (CallArgs&&... call_args) -> bool
          {
               return (... && std::invoke(f, call_args...));
          };
};


} // namespace fo
} // namespace Pattern
