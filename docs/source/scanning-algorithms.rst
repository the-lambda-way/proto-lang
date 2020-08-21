************************************************************************************************************************
Scanning Algorithms
************************************************************************************************************************

========================================================================================================================
scan, scan_if
========================================================================================================================
Given a mutable range, advances its iterator if its referenced value satisfies a condition.


Synopsis
------------------------------------------------------------
1) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::iter_value_t<I>>
     bool scan (I& first, S last, E element) const

Advances ``first`` by one if its dereferenced value equals ``element``.

2) .. code-block::

     template <mutable_forward_range R, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::ranges::range_value_t<R>>
     bool scan (R&& r, E element) const

Same as (1), using ``r`` as the source range.

3) .. code-block::

     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool scan (I1& first1, S1 last1, I2 first2, S2 last2) const

Advances ``first1`` if ``[first1, last1)`` begins with the elements of ``[first2, last2)``. After a successful match, ``first1`` has been advanced by the size of ``[first2, last2)``.

4) .. code-block::

     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan (R&& r, I first, S last) const

Same as (3), using ``r`` as the source range.

5) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan (I& first, S last, R&& r) const

Same as (3), using ``r`` as the comparison range.

6) .. code-block::

     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool scan (R1&& r1, R2&& r2) const

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) .. code-block::

     template <class CharT, std::forward_iterator I, std::sentinel_for<I> S>
          requires std::equality_comparable_with<CharT, std::iter_value_t<I>>
     bool scan (I& first, S last, const CharT* comparison) const

Same as (3), using ``std::string_view {comparison}`` as the comparison range.

8) .. code-block::

     template <class CharT, mutable_forward_range R>
          requires std::equality_comparable_with<CharT, std::ranges::range_value_t<R>>
     bool scan (R&& r, const CharT* comparison) const

Same as (3), using ``r`` as the source range and ``std::string_view {comparision}`` as the comparison range.

9) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool scan_if (I& first, S last, P pred) const

Advances ``first`` by one if ``std::invoke(pred, *first)`` returns true;

10) .. code-block::

     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::iterator_t<R>> P>
     bool scan (R&& r, P pred) const

Same as (9), using ``r`` as the source range.


Returns
------------------------------------------------------------
``true`` if the condition was satisfied, ``false`` otherwise.


Side effects
------------------------------------------------------------
May advance the iterator of the source range one or more times, using the increment operator.


Complexity
------------------------------------------------------------
| 1, 2) O(1)
| 3-8) O(n), where n is the number of elements in ``[first2, last2)``
| 9, 10) O(1) + one invocation of ``pred``


Examples
------------------------------------------------------------

.. code-block::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view source = "Programs must be written for people to read, "
                             "and only incidentally for machines to execute. "
                             "-- Harold Abelson";

          scan(source, 'P');
          std::cout << "Single element: " << *source << '\n';

          scan(source, "rograms m");
          std::cout << "Range of elements: " << *source << '\n';

          scan_if(source, [] (char c) { return c == 'u'; });
          std::cout << "Predicate: " << *source << '\n';
     }

Output

.. code-block:: text

     Single element: r
     Range of elements: u
     Predicate: s


========================================================================================================================
scan_not, scan_if_not
========================================================================================================================
Given a mutable range, advances its iterator by one if its dereferenced value fails to satisfy a condition.


Synopsis
------------------------------------------------------------
1) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::iter_value_t<I>>
     bool scan_not (I& first, S last, E element) const

Advances ``first`` by one if its dereferenced value doesn't equal ``element``.

2) .. code-block::

     template <mutable_forward_range R, class E>
          requires (!std::ranges::range<E>) &&     // reject potential equality overload between a range and an element
                   std::equality_comparable_with<E, std::ranges::range_value_t<R>>
     bool scan_not (R&& r, E element) const

Same as (1), using ``r`` as the source range.

3) .. code-block::

     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool scan_not (I1& first1, S1 last1, I2 first2, S2 last2) const

Advances ``first1`` by one if ``[first1, last1)`` doesn't begin with the elements of ``[first2, last2)``.

4) .. code-block::

     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan_not (R&& r, I first, S last) const

Same as (3), using ``r`` as the source range.

5) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan_not (I& first, S last, R&& r) const

Same as (3), using ``r`` as the comparison range.

6) .. code-block::

     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool scan_not (R1&& r1, R2&& r2) const

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) .. code-block::

     template <class CharT, std::forward_iterator I, std::sentinel_for<I> S>
          requires std::equality_comparable_with<CharT, std::iter_value_t<I>>
     bool scan_not (I& first, S last, const CharT* comparison) const

Same as (3), using ``std::string_view {comparison}`` as the comparison range.

8) .. code-block::

     template <class CharT, mutable_forward_range R>
          requires std::equality_comparable_with<CharT, std::ranges::range_value_t<R>>
     bool scan_not (R&& r, const CharT* comparison) const

Same as (3), using ``r`` as the source range and ``std::string_view {comparision}`` as the comparison range.

9) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool scan_if_not (I& first, S last, P pred) const

Advances ``first`` by one if ``std::invoke(pred, *first)`` returns ``false``.

10) .. code-block::

     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::iterator_t<R>> P>
     bool scan_if_not (R&& r, P pred) const

Same as (9), using ``r`` as the source range.

11) .. code-block::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               boolean_invocable<I, S> F>
     bool scan_if_not (I& first, S last, F f) const

Advances ``first`` by one if ``std::invoke(f, copy, last)`` returns ``false``, where ``copy`` is a copy of ``first``.

12) .. code-block::

     template <mutable_forward_range R, boolean_invocable<R>>
     bool scan_if_not (R&& r, F f) const

Same as (11), using ``r`` as the source range.


Returns
------------------------------------------------------------
``true`` if the condition was not satisfied, ``false`` otherwise.


Side effects
------------------------------------------------------------
May advance the iterator of the source range one time, using the increment operator.


Complexity
------------------------------------------------------------
| 1, 2) O(1)
| 3-8) O(n), where n is the number of elements in ``[first2, last2)``
| 9-12) O(1) + one invocation of ``pred`` or ``f``


Examples
------------------------------------------------------------

.. code-block::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view source = "Talk is cheap. Show me the code. -- Linus Torvalds";

          scan_not(source, 'Q');
          std::cout << "Single element: " << *source << '\n';

          scan_not(source, "alks");
          std::cout << "Range of elements: " << *source << '\n';

          auto equals_foo = [] (scan_view& s) { return scan(s, "foo"); };
          scan_if_not(source, equals_foo);
          std::cout << "Function: " << *source << '\n';
     }

Output

.. code-block:: text

     Single element: a
     Range of elements: l
     Function: k
