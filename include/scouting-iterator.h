#ifndef SCOUTING_ITERATOR
#define SCOUTING_ITERATOR

#include <iterator>    // type traits


// A scouting iterator is a pair of iterators designed for 2-stage traversal, where one iterator progresses while the
//     other maintains a previous position.
// Can be used as a range, if you wish to iterate over the span between the two iterators.

// Based on the design of __normal_iterator from GCC 9 (stl_iterator.h)
// Container is used to distinguish iterators of containers with the same underlying type
template <typename Iter, typename Container>
class scouting_iterator
{
    using traits_type = std::iterator_traits<Iter>;

    template<typename _Iter>
    using convertible_type = scouting_iterator<
        _Iter,
        typename std::enable_if<
            std::is_same_v<_Iter, typename Container::pointer>,
            Container
            >::type
        >;

    using self_type = scouting_iterator<Iter, Container>;

    Iter sentry = Iter {};
    Iter scout  = Iter {};

public:
    // Traits
    using iterator_category = traits_type::iterator_category;
    using value_type        = traits_type::value_type;
    using pointer           = traits_type::pointer;
    using reference         = traits_type::reference;
    using difference_type   = traits_type::difference_type;
    using size_type         = std::size_t;


    // Constructors
    constexpr scouting_iterator () noexcept
        : sentry {Iter()}, scout {sentry}
    {}

    explicit constexpr scouting_iterator (const char* i) noexcept
        : sentry {i}, scout {sentry}
    {}

    // Allow iterator to const_iterator conversion
    template <typename _Iter>
    constexpr scouting_iterator (const convertible_type<_Iter>& i) noexcept
        : sentry {i.base()}, scout {sentry}
    {}

    self_type& operator= (const Iter& i) noexcept
    {
        scout = i;
        return *this;
    }


    // Element access
    self_type& save () noexcept
    {
        sentry = scout;
        return *this;
    }

    constexpr self_type& restore () noexcept
    {
        scout = sentry;
        return *this;
    }

    constexpr const Iter& get_sentry () const noexcept    { return sentry; }
    constexpr const Iter& get_scout  () const noexcept    { return scout;  }
    constexpr const Iter& begin      () const noexcept    { return sentry; }
    constexpr const Iter& end        () const noexcept    { return scout;  }
    constexpr const Iter& data       () const noexcept    { return scout;  }
    constexpr const Iter& base       () const noexcept    { return scout;  }


    // Operations
    reference operator*  () const noexcept    { return *scout; }
    pointer   operator-> () const noexcept    { return scout;  }

    self_type& operator++ () noexcept
    {
        ++scout;
        return *this;
    }

    self_type operator++ (int) noexcept
    {
        return self_type {scout++};
    }

    self_type& operator-- () noexcept
    {
        --scout;
        return *this;
    }

    self_type operator-- (int) noexcept
    {
        return self_type {scout--};
    }

    reference operator[] (difference_type n) const noexcept
    {
        return scout[n];
    }

    self_type& operator+= (difference_type n) noexcept
    {
        scout += n;
        return *this;
    }

    self_type operator+ (difference_type n) const noexcept
    {
        return self_type {scout + n};
    }

    friend self_type operator+ (difference_type n, const self_type& i) noexcept
    {
        return self_type {i.base() + n};
    }

    self_type& operator-= (difference_type n) noexcept
    {
        scout -= n;
        return *this;
    }

    self_type operator- (difference_type n)
    {
        return self_type {scout - n};
    }

    template <typename _Iter>
    difference_type operator- (const _Iter& i) const noexcept
    {
        return scout - i;
    }

    template <typename _Iter>
    friend difference_type operator- (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs - rhs.base();
    }

    template <typename _Iter>
    bool operator== (const _Iter& i) const noexcept
    {
        return scout == i;
    }

    template <typename _Iter>
    friend bool operator== (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs == rhs.base();
    }

    template <typename _Iter>
    bool operator!= (const _Iter& i) const noexcept
    {
        return scout != i;
    }

    template <typename _Iter>
    friend bool operator!= (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs != rhs.base();
    }

    template <typename _Iter>
    bool operator< (const _Iter& i) const noexcept
    {
        return scout < i;
    }

    template <typename _Iter>
    friend bool operator< (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs < rhs.base();
    }

    template <typename _Iter>
    bool operator<= (const _Iter& i) const noexcept
    {
        return scout <= i;
    }

    template <typename _Iter>
    friend bool operator<= (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs <= rhs.base();
    }

    template <typename _Iter>
    bool operator> (const _Iter& i) const noexcept
    {
        return scout > i;
    }

    template <typename _Iter>
    friend bool operator> (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs > rhs.base();
    }

    template <typename _Iter>
    bool operator>= (const _Iter& i) const noexcept
    {
        return scout >= i;
    }

    template <typename _Iter>
    friend bool operator>= (const _Iter& lhs, const self_type& rhs) noexcept
    {
        return lhs >= rhs.base();
    }

    constexpr difference_type distance () const noexcept
    {
        return scout - sentry;
    }
};


template <typename IterL, typename IterR, typename Container>
inline bool operator== (const scouting_iterator<IterL, Container>& lhs,
                        const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() == rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline bool operator!= (const scouting_iterator<IterL, Container>& lhs,
                        const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline bool operator< (const scouting_iterator<IterL, Container>& lhs,
                       const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline bool operator<= (const scouting_iterator<IterL, Container>& lhs,
                        const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() <= rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline bool operator> (const scouting_iterator<IterL, Container>& lhs,
                       const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() > rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline bool operator>= (const scouting_iterator<IterL, Container>& lhs,
                        const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() >= rhs.base();
}

template <typename IterL, typename IterR, typename Container>
inline typename scouting_iterator<IterL, Container>::difference_type
operator- (const scouting_iterator<IterL, Container>& lhs,
           const scouting_iterator<IterR, Container>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}


#endif // SCOUTING_ITERATOR
