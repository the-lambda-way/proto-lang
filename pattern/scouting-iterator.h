#ifndef SCOUTING_ITERATOR
#define SCOUTING_ITERATOR

#include <iterator>    // type traits


// ---------------------------------------------------------------------------------------------------------------------
// Scouting Iterator
// ---------------------------------------------------------------------------------------------------------------------

// A scouting iterator is a pair of iterators designed for 2-stage traversal, where one iterator progresses while the
//     other maintains a previous position.

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
     using iterator_category = typename traits_type::iterator_category;
     using value_type        = typename traits_type::value_type;
     using pointer           = typename traits_type::pointer;
     using reference         = typename traits_type::reference;
     using difference_type   = typename traits_type::difference_type;
     using size_type         = std::size_t;


     // Constructors
     constexpr scouting_iterator () noexcept
          : sentry {Iter()}, scout {sentry}
     {}

     explicit constexpr scouting_iterator (const value_type* i) noexcept
          : sentry {i}, scout {sentry}
     {}

     explicit constexpr scouting_iterator (const Iter& sentry, const Iter& scout) noexcept
          : sentry {sentry}, scout {scout}
     {}

     // Allow iterator to const_iterator conversion
     template <typename _Iter>
     constexpr scouting_iterator (const convertible_type<_Iter>& i) noexcept
          : sentry {i.get_sentry()}, scout {sentry}
     {}

     template <typename _Iter>
     self_type& operator= (const convertible_type<_Iter>& i) noexcept
     {
          scout  = i.scout;
          sentry = i.sentry;
          return *this;
     }

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

     constexpr Iter  get_sentry  () const noexcept    { return sentry; }
     constexpr Iter  get_scout   () const noexcept    { return scout;  }
     constexpr Iter& sentry_base ()       noexcept    { return sentry; }
     constexpr Iter& base        ()       noexcept    { return scout;  }
     constexpr const value_type* saved_data () const noexcept    { return &*sentry; }
     constexpr const value_type* data       () const noexcept    { return &*scout; }


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
          return scouting_iterator {sentry, scout++};
     }

     self_type& operator-- () noexcept
     {
          --scout;
          return *this;
     }

     self_type operator-- (int) noexcept
     {
          return scouting_iterator {sentry, scout--};
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
          return {sentry, scout + n};
     }

     friend self_type operator+ (difference_type n, const self_type& i) noexcept
     {
          return {i.sentry, i.scout + n};
     }

     self_type& operator-= (difference_type n) noexcept
     {
          scout -= n;
          return *this;
     }

     self_type operator- (difference_type n)
     {
          return {sentry, scout - n};
     }

     difference_type operator- (const Iter& i) const noexcept
     {
          return scout - i;
     }

     friend difference_type operator- (const Iter& lhs, const self_type& rhs) noexcept
     {
          return lhs - rhs.scout;
     }

     std::weak_ordering operator<=> (self_type rhs) const noexcept     { return scout <=> rhs.scout; }
     bool               operator==  (self_type rhs) const noexcept     { return scout ==  rhs.scout; }
     std::weak_ordering operator<=> (Iter i)        const noexcept     { return scout <=> i;         }
     bool               operator==  (Iter i)        const noexcept     { return scout ==  i;         }

     constexpr difference_type distance () const noexcept
     {
          return scout - sentry;
     }
}; // scouting_iterator


template <typename IterL, typename IterR, typename Container>
inline typename scouting_iterator<IterL, Container>::difference_type
operator- (const scouting_iterator<IterL, Container>& lhs,
           const scouting_iterator<IterR, Container>& rhs) noexcept
{
     return lhs.get_scout() - rhs.get_scout();
}


// // ---------------------------------------------------------------------------------------------------------------------
// // Speculative Iterator
// // ---------------------------------------------------------------------------------------------------------------------
// // A speculative iterator maintains a stack of iterators for speculative advancement through a sequence.
// // Container is used to distinguish iterators of containers with the same underlying type
// template <typename Iter, typename Container>
// class speculative_iterator
// {
//     using traits_type = std::iterator_traits<Iter>;

//     template<typename _Iter>
//     using convertible_type = speculative_iterator<
//         _Iter,
//         typename std::enable_if<
//             std::is_same_v<_Iter, typename Container::pointer>,
//             Container
//             >::type
//         >;

//     using self_type = speculative_iterator<Iter, Container>;

//     std::vector<Iter> iters;
//     Iter              current;


//     // think about how to manage input iterators properly
//     // -- the book recommends never clearing the container while speculating,
//     // -- this should work

//     // and about implementing actual lookahead
//     // -- easy enough by never letting the pointer go pass a certain point before
//     // -- the end, however, clearing the container will be more expensive, since
//     // -- we'd have to copy the lookahead, since we might be using an input iterator





// public:
//     // Traits
//     using iterator_category = typename traits_type::iterator_category;
//     using value_type        = typename traits_type::value_type;
//     using pointer           = typename traits_type::pointer;
//     using reference         = typename traits_type::reference;
//     using difference_type   = typename traits_type::difference_type;
//     using size_type         = std::size_t;


//     // Constructors
//     constexpr speculative_iterator () noexcept
//         : iters {Iter()}, current {iters.front()}
//     {}

//     template <typename ValueType>
//     explicit constexpr speculative_iterator (const ValueType*... is) noexcept
//         : iters {is...}, current {iters.front()}
//     {}

//     // Allow iterator to const_iterator conversion
//     template <typename _Iter>
//     constexpr speculative_iterator (const convertible_type<_Iter>& i) noexcept
//         : iters {i.iters}, current {current}
//     {}

//     template <typename _Iter>
//     self_type& operator= (const convertible_type<_Iter>& i) noexcept
//     {
//         iters = i.iters;
//         return *this;
//     }

//     // Element access
//     self_type& save () noexcept
//     {
//         iters.push_back(iters.back());
//         return *this;
//     }

//     constexpr self_type& restore () noexcept
//     {
//         iters.pop_back();
//         return *this;
//     }


//     // Operations
//     reference operator*  () const noexcept    { return *current; }
//     pointer   operator-> () const noexcept    { return current;  }

//     self_type& operator++ () noexcept
//     {
//         advance();
//         return *this;
//     }

//     self_type operator++ (int) noexcept
//     {
//         sync(1);                  // can't assume a forward iterator, so sync before copying
//         self_type copy = *this;
//         advance();

//         return copy;
//     }

//     self_type& operator-- () noexcept
//     {
//         --current;
//         return *this;
//     }

//     self_type operator-- (int) noexcept
//     {
//         return {iters, current--};
//     }

//      self_type& operator+= (difference_type n) noexcept
//     {
//         advance(n)
//         return *this;
//     }

//     self_type operator+ (difference_type n) const noexcept
//     {
//         self_type copy = *this;
//         copy.advance(n);
//         return copy;
//     }

//     friend self_type operator+ (difference_type n, const self_type& i) noexcept
//     {
//         return i + n;
//     }

//     self_type& operator-= (difference_type n) noexcept
//     {
//         current -= n;
//         return *this;
//     }

//     self_type operator- (difference_type n)
//     {
//         self_type copy = *this;
//         copy -= n;
//         return copy;
//     }

//     difference_type operator- (const Iter& i) const noexcept
//     {
//         return current - i.current;
//     }

//     friend difference_type operator- (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs - rhs.scout;
//     }

//     bool operator== (const Iter& i) const noexcept
//     {
//         return current = i.current;
//     }

//     friend bool operator== (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs == rhs.current;
//     }

//     bool operator!= (const Iter& i) const noexcept
//     {
//         return current != i.current;
//     }

//     friend bool operator!= (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs != rhs.current;
//     }

//     bool operator< (const Iter& i) const noexcept
//     {
//         return current < i.current;
//     }

//     friend bool operator< (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs < rhs.current;
//     }

//     bool operator<= (const Iter& i) const noexcept
//     {
//         return current <= i.current;
//     }

//     friend bool operator<= (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs <= rhs.current;
//     }

//     bool operator> (const Iter& i) const noexcept
//     {
//         return current > i.current;
//     }

//     friend bool operator> (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs > rhs.current;
//     }

//     bool operator>= (const Iter& i) const noexcept
//     {
//         return current >= i.current;
//     }

//     friend bool operator>= (const Iter& lhs, const self_type& rhs) noexcept
//     {
//         return lhs >= rhs.current;
//     }

// private:
//     void advance (int n = 1)
//     {
//         ++current;

//         if (current = lookahead.size() && iters.empty())
//         {
//             lookahead.clear();
//             current = lookahead.front();
//         }

//         sync(1);
//     }

//     void sync (int i)
//     {
//         int diff = lookahead.back() - current + i;
//         if (diff > 0)    fill(diff);
//     }

//     void fill (int n)
//     {
//         while (n--)    lookahead.push_back(*(++current););
//     }

// }; // speculative_iterator


// template <typename IterL, typename IterR, typename Container>
// inline bool operator== (const speculative_iterator<IterL, Container>& lhs,
//                         const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current == rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline bool operator!= (const speculative_iterator<IterL, Container>& lhs,
//                         const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current != rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline bool operator< (const speculative_iterator<IterL, Container>& lhs,
//                        const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current < rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline bool operator<= (const speculative_iterator<IterL, Container>& lhs,
//                         const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current <= rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline bool operator> (const speculative_iterator<IterL, Container>& lhs,
//                        const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current > rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline bool operator>= (const speculative_iterator<IterL, Container>& lhs,
//                         const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current >= rhs.current;
// }

// template <typename IterL, typename IterR, typename Container>
// inline typename speculative_iterator<IterL, Container>::difference_type
// operator- (const speculative_iterator<IterL, Container>& lhs,
//            const speculative_iterator<IterR, Container>& rhs) noexcept
// {
//     return lhs.current - rhs.current;
// }

#endif // SCOUTING_ITERATOR
