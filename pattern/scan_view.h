/*
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Scan View
 *
 * A mutable range useful for scanning a character sequence.
 *
 */



// Think again about whether this is the wrong abstraction level. Scouting iterator can handle the paired iterator
// features, while a scanning object should perhaps provide more additional features than simply substr and copy.
//






#pragma once

#include <algorithm>       // std::min
#include <compare>         // std::weak_ordering
#include <stdexcept>       // std::out_of_range
#include <string>
#include <string_view>
#include <type_traits>     // static asserts


namespace Pattern {

// Based on the design of string_view from GCC 9
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_scan_view
{
     static_assert(!std::is_array_v<CharT>);
     static_assert(std::is_trivial_v<CharT> && std::is_standard_layout_v<CharT>);
     static_assert(std::is_same_v<CharT, typename Traits::char_type>);

     using iter_traits    = std::iterator_traits<const CharT*>;
     using iterator_type  = typename iter_traits::pointer;
     using self_type      = basic_scan_view<CharT, Traits>;

     iterator_type retainer;
     iterator_type cursor;
     iterator_type last;


public:
     // Iterator Traits
     using iterator_category = typename iter_traits::iterator_category;
     using value_type        = typename iter_traits::value_type;
     using pointer           = typename iter_traits::pointer;
     using reference         = typename iter_traits::reference;
     using difference_type   = typename iter_traits::difference_type;

     // Container traits
     using traits_type            = Traits;
     using const_pointer          = const pointer;
     using const_reference        = const reference;
     using const_iterator         = const iterator_type;
     using iterator               = iterator_type;
     using const_reverse_iterator = std::reverse_iterator<const_iterator>;
     using reverse_iterator       = const_reverse_iterator;
     using size_type		    = difference_type;

     static constexpr size_type npos = size_type(-1);


     // --------------------------------------------------
     // Initialization
     // --------------------------------------------------
     constexpr basic_scan_view () noexcept
          : retainer {nullptr}, cursor {nullptr}, last {nullptr}
     {}

     constexpr basic_scan_view (const basic_scan_view&) noexcept = default;

     constexpr basic_scan_view (const CharT* str) noexcept
          : retainer {str}, cursor {str}, last {str + traits_type::length(str)}
     {}

     constexpr basic_scan_view (const CharT* str, size_type length) noexcept
          : retainer {str}, cursor {retainer}, last {str + length}
     {}

     constexpr basic_scan_view (iterator_type retainer, iterator_type cursor, iterator_type last)
          : retainer {retainer}, cursor {cursor}, last {last}
     {}

     template <std::contiguous_iterator I, std::sized_sentinel_for<I> S>
          requires std::same_as<std::iter_value_t<I>, CharT> &&
                   (!std::convertible_to<S, size_type>)
     constexpr basic_scan_view (I first, S last)
          : retainer {std::to_address(first)}, cursor {retainer}, last {std::to_address(last)}
     {}

     constexpr basic_scan_view (std::string_view str) noexcept
          : retainer {str.begin()}, cursor {retainer}, last {str.end()}
     {}

     constexpr basic_scan_view& operator= (const basic_scan_view&) noexcept = default;


     // --------------------------------------------------
     // Iterators
     // --------------------------------------------------
     constexpr iterator&              begin   ()       noexcept     { return cursor;   }
     constexpr iterator               end     () const noexcept     { return last;     }
     constexpr const_iterator&        cbegin  () const noexcept     { return cursor;   }
     constexpr const_iterator         cend    () const noexcept     { return last;     }
     constexpr reverse_iterator       rbegin  ()       noexcept     { return {last};   }
     constexpr reverse_iterator       rend    () const noexcept     { return {cursor}; }
     constexpr const_reverse_iterator crbegin () const noexcept     { return {last};   }
     constexpr const_reverse_iterator crend   () const noexcept     { return {cursor}; }


     // --------------------------------------------------
     // Capacity
     // --------------------------------------------------
     constexpr size_type size     () const noexcept     { return last - cursor;  }
     constexpr size_type length   () const noexcept     { return last - cursor;  }
     constexpr bool      empty    () const noexcept     { return cursor == last; }
     constexpr bool      has_more () const noexcept     { return cursor != last; }


     // --------------------------------------------------
     // Element Access
     // --------------------------------------------------
     constexpr const_pointer data () const noexcept     { return cursor; }

     constexpr value_type operator[] (size_type n) const noexcept
     {
          return *(cursor + n);
     }

     constexpr value_type at (size_type n) const
     {
          if (n >= size())     throw std::out_of_range("scan_view::at: n >= this->size()");
          return *(cursor + n);
     }

     constexpr value_type peek () const noexcept
     {
          return *cursor;
     }

     constexpr value_type next () const noexcept
     {
          return *(++cursor);
     }

     constexpr std::string_view lookahead (size_type n) const
     {
          return {cursor, std::min(n, size())};
     }


     // --------------------------------------------------
     // Modifiers
     // --------------------------------------------------
     constexpr void swap (basic_scan_view& other) noexcept
     {
          auto tmp = *this;
          *this = other;
          other = tmp;
     }


     // --------------------------------------------------
     // Iterator Operations
     // --------------------------------------------------
     constexpr value_type operator*  () const noexcept     { return *cursor; }
     constexpr pointer    operator-> () const noexcept     { return cursor;  }

     constexpr self_type& increment      ()                           { ++cursor; return *this;            }
     constexpr self_type  post_increment ()                           { return {retainer, cursor++, last}; }
     constexpr self_type& advance        (difference_type n = 1)      { cursor += n; return *this;         }
     constexpr self_type& decrement      ()                           { --cursor; return *this;            }
     constexpr self_type  post_decrement ()                           { return {retainer, cursor--, last}; }
     constexpr self_type& backtrack      (difference_type n = 1)      { cursor -= n; return *this;         }

     constexpr self_type& operator++ ()                       { return increment();      }
     constexpr self_type  operator++ (int)                    { return post_increment(); }
     constexpr self_type& operator+= (difference_type n)      { return advance(n);       }
     constexpr self_type& operator-- ()                       { return decrement();      }
     constexpr self_type  operator-- (int)                    { return post_decrement(); }
     constexpr self_type& operator-= (difference_type n)      { return backtrack(n);     }

     friend constexpr self_type operator+ (self_type s, difference_type n)     { return s += n; }
     friend constexpr self_type operator+ (difference_type n, self_type s)     { return s += n; }
     friend constexpr self_type operator- (self_type s, difference_type n)     { return s -= n; }

     friend constexpr difference_type operator-(self_type s, const_iterator i)     { return s.cursor - i; }
     friend constexpr difference_type operator-(const_iterator i, self_type s)     { return i - s.cursor; }

     constexpr std::weak_ordering operator<=> (const self_type& other) const     { return cursor <=> other.cursor; }
     constexpr bool               operator==  (const self_type& other) const     { return cursor == other.cursor;  }

     constexpr self_type& save    ()     { retainer = cursor; return *this; }
     constexpr self_type& restore ()     { cursor = retainer; return *this; }


     // --------------------------------------------------
     // Range Operations
     // --------------------------------------------------
     constexpr size_type copy (CharT* dest, size_type count, size_type pos = 0) const
     {
          if (pos > size())     throw std::out_of_range("basic_scan_view::copy: pos > size()");

          const size_type rcount = std::min(count, size() - pos);

          traits_type::copy(dest, cursor + pos, rcount);
          return rcount;
     }

     constexpr std::string_view substr (size_type pos = 0, size_type count = npos) const noexcept(false)
     {
          if (pos > size())     throw std::out_of_range("basic_scan_view::substr: pos > size()");
          return {cursor + pos, std::min(count, size() - pos)};
     }

     constexpr std::string_view skipped (size_type from_front = 0, size_type from_back = 0) const noexcept(false)
     {
          if (from_front > size())     throw std::out_of_range("basic_scan_view::skipped: from_front > size()");
          return {retainer + from_front, cursor - from_front - from_back};
     }

     constexpr std::string copy_skipped (size_type from_front = 0, size_type from_back = 0) const noexcept(false)
     {
          if (from_front > size())     throw std::out_of_range("basic_scan_view::copy_skipped: from_front > size()");
          return {retainer + from_front, cursor - from_front - from_back};
     }
};

using scan_view = basic_scan_view<char>;

} // namespace Pattern
