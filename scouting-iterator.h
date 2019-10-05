#ifndef SCOUTING_ITERATOR
#define SCOUTING_ITERATOR

#include <iterator>    // type traits


// A scouting iterator is a pair of iterators designed for 2-stage traversal, where one iterator progresses
//     speculatively while the other maintains a previous position.
// Can be used as a range, if you wish to iterate over the span between the two iterators.

// Based on the design of __normal_iterator from GCC 9 (stl_iterator.h)
// Container is used to distinguish iterators of containers with the same underlying type
template <typename Iter, typename Container>
class scouting_iterator
{
protected:
    using traits_type      = std::iterator_traits<Iter>;
    using pointer_type     = traits_type::pointer;
    using convertible_type = scouting_iterator<
        Iter,
        typename std::enable_if<std::is_same_v<Iter, typename Container::pointer>,
                                Container::type>;

    pointer_type scout;
    pointer_type sentry;

public:
    // Types
    using iterator_category      = traits_type::iterator_category;
    using iterator               = std::iterator_type;
    using const_iterator         = const iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using value_type             = traits_type::value_type;
    using difference_type        = traits_type::difference_type;
    using reference              = traits_type::reference;
    using const_reference        = const reference;
    using pointer                = pointer_type;
    using const_pointer          = const pointer*;


    // Constructors
    constexpr scouting_iterator () noexcept
        : sentry {Iter()}, scout {sentry}
    {}

    explicit constexpr scouting_iterator (const_reference i) noexcept
        : sentry {i}, scout {sentry}
    {}

    constexpr scouting_iterator (const convertible_type& i) noexcept
        : sentry {i.sentry()}, scout {sentry}
    {}

    constexpr reference operator= (pointer p)
    {
        scout = p;
    }


    // Operations
    constexpr reference operator* () const noexcept
    {
        return *scout;
    }

    constexpr pointer operator-> () const noexcept
    {
        return scout;
    }

    constexpr reference operator++ () noexcept
    {
        return *(++scout);
    }

    constexpr value_type operator++ (int) noexcept
    {
        value_type tmp = *scout;
        operator++();
        return tmp;
    }

    constexpr reference operator-- () noexcept
    {
        return *(--scout);
    }

    constexpr value_type operator-- (int) noexcept
    {
        value_type tmp = *scout;
        operator--();
        return tmp;
    }

    constexpr reference operator[] (size_type n) const noexcept
    {
        return scout[n];
    }

    constexpr reference operator+= (difference_type n) noexcept
    {
        return *(scout += n);
    }

    friend constexpr value_type operator+ (convertible_type i, difference_type n) const noexcept
    {
        return i += n;
    }

    friend constexpr value_type operator+ (difference_type n, convertible_type i) const noexcept
    {
        return i += n;
    }

    constexpr reference operator-= (difference_type n) noexcept
    {
        return *(scout -= n);
    }

    friend constexpr value_type operator- (convertible_type i, difference_type n) const noexcept
    {
        return i -= n;
    }

    friend constexpr difference_type operator- (convertible_type lhs, const convertible_type& rhs)
    {
        return lhs.scout - rhs.scout;
    }

    friend constexpr difference_type operator- (convertible_type lhs, const value_type& rhs)
    {
        return lhs.scout - rhs;
    }

    friend constexpr difference_type operator- (value_type lhs, const convertible_type& rhs)
    {
        return lhs - rhs.scout;
    }

    friend constexpr bool operator <=> (const convertible_type& lhs, const convertible_type& rhs)
    {
        return lhs.scout <=> rhs.scout;
    }

    constexpr reference save () noexcept
    {
        sentry = scout;
        return sentry;
    }

    constexpr reference restore () noexcept
    {
        scout = sentry;
        return scout;
    }

    constexpr reference restore (reference i)
    {
        scout = i;
        return scout;
    }

    constexpr reference sentry () const noexcept
    {
        return sentry;
    }

    constexpr reference begin () const noexcept
    {
        return sentry;
    }

    constexpr reference scout () const noexcept
    {
        return scout;
    }

    constexpr reference end () const noexcept
    {
        return scout;
    }
    
    constexpr reference get () const noexcept
    {
        return scout;
    }

    constexpr difference_type distance () const noexcept
    {
        return scout - sentry;
    }
};


#endif // SCOUTING_ITERATOR