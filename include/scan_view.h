#ifndef SCANNER
#define SCANNER

#include <string>
#include <string_view>
#include "../include/scouting-iterator.h"
#include "../include/scanning-algorithms.h"
#include "../include/syntax.h"


// A non-owning reference to a character sequence, with iterator semantics.
// Based on the design of string_view from GCC 9
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_scan_view
{
    using iter_traits    = std::iterator_traits<const CharT*>;
    using container_type = std::basic_string_view<CharT, Traits>;
    using iterator_type  = scouting_iterator<typename iter_traits::pointer, container_type>;
    using self_type      = basic_scan_view<CharT, Traits>;

    container_type sequence;
    iterator_type  cursor;

public:
    // Iterator Traits
    using iterator_category = iter_traits::iterator_category;
    using value_type        = iter_traits::value_type;
    using pointer           = iter_traits::pointer;
    using reference         = iter_traits::reference;
    using difference_type   = iter_traits::difference_type;

    // Container traits
    using traits_type            = Traits;
    using const_pointer          = const value_type*;
    using const_reference        = const value_type&;
    using const_iterator         = const iterator_type;
    using iterator               = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = const_reverse_iterator;
    using size_type		         = difference_type;

    static constexpr size_type npos = size_type(-1);


    // ------------------------
    // Initialization
    // ------------------------
    constexpr basic_scan_view () noexcept : sequence {}, cursor {} {}

    constexpr basic_scan_view (const basic_scan_view&) noexcept = default;

    constexpr basic_scan_view (container_type sequence) noexcept
        : sequence {sequence}, cursor {sequence.begin()}
    {}

    constexpr basic_scan_view (container_type sequence, iterator_type position) noexcept
        : sequence {sequence}, cursor {position}
    {}

    constexpr basic_scan_view& operator= (const basic_scan_view&) noexcept = default;


    // ------------------------
    // Relative Iterators
    // ------------------------
    constexpr const_iterator begin   () const noexcept    { return cursor.base();  }
    constexpr const_iterator end     () const noexcept    { return sequence.end(); }
    constexpr const_iterator cbegin  () const noexcept    { return cursor.base();  }
    constexpr const_iterator cend    () const noexcept    { return sequence.end(); }
    constexpr const_iterator rbegin  () const noexcept    { return const_reverse_iterator {cursor.base()};  }
    constexpr const_iterator rend    () const noexcept    { return const_reverse_iterator {sequence.end()}; }
    constexpr const_iterator crbegin () const noexcept    { return const_reverse_iterator {cursor.base()};  }
    constexpr const_iterator crend   () const noexcept    { return const_reverse_iterator {sequence.end()}; }


    // ------------------------
    // Relative Capacity
    // ------------------------
    constexpr size_type length   () const noexcept    { return sequence.end() - cursor;  }
    constexpr bool      eof      () const noexcept    { return cursor == sequence.end(); }
    constexpr bool      has_more () const noexcept    { return cursor != sequence.end(); }


    // ------------------------
    // Relative Element Access
    // ------------------------
    constexpr const_pointer basis () const noexcept    { return sequence.data(); }
    constexpr const_pointer data  () const noexcept    { return cursor.data(); }

    constexpr value_type operator[] (size_type n) const noexcept
    {
        if (eof())    return '\0';
        return cursor[n];
    }

    constexpr value_type peek () const noexcept
    {
        if (eof())    return '\0';
        return *cursor;
    }

    constexpr value_type peek_next () const noexcept
    {
        if (length() < 2)    return '\0';
        return *(cursor + 1);
    }

    constexpr value_type    operator*  () const    { return peek(); }
    constexpr const_pointer operator-> () const    { return cursor.operator->(); }

    constexpr self_type& increment      ()                         { ++cursor;    return *this; }
    constexpr self_type& operator++     ()                         { ++cursor;    return *this; }
    constexpr self_type  post_increment ()                         { return basic_scan_view {sequence, cursor++}; }
    constexpr self_type  operator++     (int)                      { return basic_scan_view {sequence, cursor++}; }
    constexpr self_type& advance        (difference_type n = 1)    { cursor += n; return *this; }
    constexpr self_type& operator+=     (difference_type n)        { cursor += n; return *this; }
    friend constexpr self_type operator+ (self_type s, difference_type n)    { return s += n; }
    friend constexpr self_type operator+ (difference_type n, self_type s)    { return s += n; }


    constexpr self_type& decrement      ()                         { --cursor;    return *this; }
    constexpr self_type& operator--     ()                         { --cursor;    return *this; }
    constexpr self_type  post_decrement ()                         { return basic_scan_view {sequence, cursor--}; }
    constexpr self_type  operator--     (int)                      { return basic_scan_view {sequence, cursor--}; }
    constexpr self_type& backtrack      (difference_type n = 1)    { cursor -= n; return *this; }
    constexpr self_type& operator-=     (difference_type n)        { cursor -= n; return *this; }
    friend constexpr self_type operator- (self_type s, difference_type n)    { return s -= n; }
    friend constexpr self_type operator- (difference_type n, self_type s)    { return s -= n; }


    constexpr bool operator <  (const_reference other)    { return cursor <  other.cursor; }
    constexpr bool operator <= (const_reference other)    { return cursor <= other.cursor; }
    constexpr bool operator >  (const_reference other)    { return cursor >  other.cursor; }
    constexpr bool operator >= (const_reference other)    { return cursor >= other.cursor; }
    constexpr bool operator == (const_reference other)    { return cursor == other.cursor; }
    constexpr bool operator != (const_reference other)    { return cursor != other.cursor; }


    constexpr self_type& save      ()              { cursor.save();     return *this; }
    constexpr self_type& restore   ()              { cursor.restore();  return *this; }
    constexpr self_type& restore   (iterator i)    { cursor.restore(i); return *this; }
    constexpr self_type& operator= (iterator i)    { cursor.restore(i); return *this; }


    // ------------------------
    // Operations
    // ------------------------
    // Copy "count" characters starting "cursor" characters from the current position
    constexpr size_type copy (CharT* dest, size_type count, size_type cursor = 0) const
    {
        return sequence.copy(dest, count, current_index() + cursor);
    }

    std::string to_string (size_type start = 0, size_type length = npos) const noexcept
    {
        length = std::min(length, this->length());
        return {cursor.data() + start, length};
    }

    std::string to_string (iterator first, iterator last)
    {
        return {first, last - first};
    }

    // Returns a view of length "length" starting from the current position
    constexpr container_type substr (size_type length) const noexcept(false)
    {
        return sequence.substr(current_index(), length);
    }

    // Returns a view of length "length" starting "start" characters from the current position
    constexpr container_type substr (size_type start, size_type length) const noexcept(false)
    {
        return sequence.substr(current_index() + start, length);
    }

    // Returns a view of characters starting at "first" and ending at "last"
    constexpr container_type substr (iterator first, iterator last)
    {
        return sequence.substr(first - sequence.begin(), last - first);
    }

    // Returns a view of characters from the last saved position to the current position, shrinking by from_front and
    // from_back
    constexpr container_type skipped (size_type from_front = 0, size_type from_back = 0)
    {
        return sequence.substr(base_index() + from_front, cursor.distance() - from_back);
    }

    constexpr std::string copy_skipped (size_type from_front = 0, size_type from_back = 0)
    {
        return {cursor.saved_data() + from_front, cursor.data() - from_back};
    }


private:
    constexpr size_type current_index () const noexcept    { return cursor - sequence.begin();              }
    constexpr size_type base_index    () const noexcept    { return cursor.get_sentry() - sequence.begin(); }
};

using scan_view = basic_scan_view<char>;


// ------------------------
// Predicates
// ------------------------
template <typename CharT, typename T>
constexpr bool operator== (basic_scan_view<CharT> s, T&& t)
{
    return starts_with(s, forward<T>(t));
}


template <typename T, typename CharT>
constexpr bool operator== (T&& t, basic_scan_view<CharT> s)
{
    return starts_with(s, forward<T>(t));
}


template <typename CharT, typename T>
constexpr bool operator!= (basic_scan_view<CharT> s, T&& t)
{
    return !starts_with(s, forward<T>(t));
}


template <typename T, typename CharT>
constexpr bool operator!= (T&& t, basic_scan_view<CharT> s)
{
    return !starts_with(s, forward<T>(t));
}


// ------------------------
// Actions
// ------------------------
template <typename CharT>
constexpr bool operator>> (basic_scan_view<CharT>& s, char c)
{
    if (s != c)    return false;
    ++s;
    return true;
}


template <typename CharT>
constexpr bool operator>> (basic_scan_view<CharT>& s, char_predicate p)
{
    if (p(*s))    return false;
    ++s;
    return true;
}


template <typename CharT>
constexpr bool operator>> (basic_scan_view<CharT>& s, string_view literal)
{
    s.update();
    if (!starts_with(&s.begin(), s.end(), literal))    return false;
    s.update();
    return true;
}


template <typename CharT, typename ScanExpr>
constexpr bool operator>> (basic_scan_view<CharT>& s, ScanExpr e)
{
    return e(s);
}


    // operator>> is apply operator?


    // Pattern language using expression templates
    // operator^ repeat
    // operator* kleene
    // operator[] access
    // operator< n min
    // n < scanner min
    // operator> n max
    // n > scanner max
    // operator== n count


#endif // SCANNER
