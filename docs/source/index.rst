.. comment
    For an automated documentation build solution, see
    https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/


The Pattern Library
===================

.. toctree::
    :maxdepth: 2
    :caption: Contents:



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`



Documentation
=============

========================================================================================================================
Overview
========================================================================================================================



========================================================================================================================
Interface
========================================================================================================================

Algorithms in this library take a source range and either a predicate which is applied to the source range, or another range to compare equal with the source. The algorithms advance the first iterator of the source range if the condition returns true, and do not modify it otherwise. Thus, begin iterators are taken by reference. Likewise, source range objects must provide a reference to its first iterator.

Two types of conditional expressions are supported.

==================  ======================  ==============  ========================================================
Expression type     Description             Range example   Predicate example
==================  ======================  ==============  ========================================================
atomic scannable    Compares the first      ``'a'``         ``bool is_char_a (char c) { return c == 'a'; }``
                    element of the source
compound scannable  Compares multiple       ``"Hello", 6``  ``bool is_hello (const char* i, const char* s) { ... }``
                    elements of the source
==================  ======================  ==============  ========================================================

*Atomic scannable expressions* which are predicates take the value of a single element. The iterator of the source is advanced by the algorithms depending on the return value of the predicate. *Compound scannable expressions* which are predicates are expected to advance the iterator themselves, as the number of elements they scan is not known by the algorithms which use them.

Source ranges, if supplied as a single object, must conform to the ``mutable_range`` concept, which ensures the begin iterator is supplied as a reference. If that is a burden, you can call the algorithms with an iterator reference and sentinel directly.


========================================================================================================================
Concepts
========================================================================================================================

------------------------------------------------------------
reference_iterator_ref
------------------------------------------------------------
A helper concept indicating a reference to an input iterator.

.. code::

    template <class I>
    concept input_iterator_ref = is_reference_v<I> && input_iterator<remove_reference<I>>;


------------------------------------------------------------
indirect_value_t
------------------------------------------------------------
A helper class providing the value type of an iterator reference.

.. code::

    template <class I>
    using indirect_value_t = iter_value_t<remove_reference<I>>;


------------------------------------------------------------
atomic_scannable_expression
------------------------------------------------------------
Conforms to the interface of atomic scanning operations, which includes expressions equality comparable with an element and predicates on the element. Specialize the template with the iterator type of your source range.

.. code::

    template <class E, class I>
    concept atomic_scannable_expression = input_iterator_ref<I> && (
        equality_comparable_with<E, indirect_value_t<I>> ||
        indirect_unary_predicate<E, remove_reference<I>>);


------------------------------------------------------------
scanning_algorithm
------------------------------------------------------------
A predicate which takes take an iterator reference and a sentinel to the source range. The predicate should advance the iterator if its condition is satisfied, and otherwise leave it unmodified.

.. code::

    template <class P, class I, class S>
    concept scanning_algorithm =
        input_iterator_ref<I> && sentinel_for<remove_reference<I>, S> &&
        predicate<P, I, S>;


------------------------------------------------------------
compound_scannable_expression
------------------------------------------------------------
Conforms to the interface of compound scanning operations, which includes expressions equality comparable with ranges and predicates on ranges.

.. code::

    template <class E, class I, class S>
    concept compound_scannable_expression =
        input_iterator_ref<I> && sentinel_for<remove_reference<I>, S> &&
        ((range<E> && equality_comparable_with<ranges::range_value_t<E>, indirect_value_t<I>)) ||
        scanning_algorithm<E, I, S>);


------------------------------------------------------------
scannable_expression
------------------------------------------------------------
Conforms to the interface of either atomic or compound scanning operations. This includes expressions equality comparable with single elements or ranges, or predicates on the same.

.. code::

    template <class E, class Iterator, class Sentinel>
    concept scannable_expression =
        atomic_scannable_expression<E, Iterator> ||
        compound_scannable_expression<E, Iterator, Sentinel>;


------------------------------------------------------------
mutable_range
------------------------------------------------------------
A refinement of ``ranges::input_range`` that provides references to its iterators.

.. code::

    template <class R>
    concept mutable_range = ranges::input_range<R> && is_reference_v<ranges::iterator_t<R>>;


========================================================================================================================
Scanning Predicates
========================================================================================================================

------------------------------------------------------------
starts_with
------------------------------------------------------------
Checks whether the first character of a string satisfies a condition. Includes a bounds check.


Synopsis
------------------------------------------------------------
1) .. code::

    template <input_iterator I, sentinel_for<I> S, class T>
        requires equality_comparable_with<T, iter_value_t<I>>
    bool starts_with (I first, S last, T t)

Checks the first character in the range beginning at ``first`` for equality with ``c``.

2) .. code::

    template <input_iterator I, sentinel_for<I> S, indirect_unary_predicate<I> P>
    bool starts_with (I first, S last, P p)

Checks if the first character in the range beginning at ``first`` satisfies ``p``.

3) .. code::

      template <class... T>
      bool starts_with (range r, T&&... t)

Same as (1, 2), but uses ``r`` as the source range.


Returns
------------------------------------------------------------
``true`` if the character satisfies the condition, ``false`` otherwise


Complexity
------------------------------------------------------------
1) Two equality checks
2) One equality check and one application of the predicate `p`
3) Same as (1, 2)


Examples
------------------------------------------------------------

.. sourcecode::

    int main ()
    {
        string s = "Hello, World!";
        cout << starts_with(s.begin(), s.end(), 'H') << "\n";
        cout << starts_with(s, [](char c){c == 'x';}) << "\n";
    }

Output

.. sourcecode::

    true
    false

.. seealso:: scan


========================================================================================================================
Element Traversal
========================================================================================================================

------------------------------------------------------------
scan
------------------------------------------------------------
Given two ranges, advances the first iterator of the first range as long as it's dereferenced value satisfies a condition.


Synopsis
------------------------------------------------------------
1) .. code::

    template <input_iterator I, sentinel_for<I> S, class... Args>
    bool scan_with (I& first, S last, atomic_scannable_expression<Args...> e, Args&&... args)


2) .. code::

    template <input_iterator I, sentinel_for<I> S>
    bool scan_with (I& first, S last, input_iterator first2)

Compares the beginning of the range ``[first, last)`` for equality with the range starting at ``first2``.


Returns
------------------------------------------------------------



Exceptions
------------------------------------------------------------



Side effects
------------------------------------------------------------



Complexity
------------------------------------------------------------
1) One equality check and one application of ``e`` (either a predicate, or an equality check).
2) :expr:`2*literal.size()` equality checks, plus :expr:`2*literal.size()` iterations.


Notes
------------------------------------------------------------



Examples
------------------------------------------------------------

.. sourcecode::



Output

.. sourcecode::



.. seealso:: advance_if


========================================================================================================================
Algorithms
========================================================================================================================

------------------------------------------------------------
advance_if
------------------------------------------------------------
Given a character range, advances the first iterator if the beginning of the range satisfies a condition. Includes a bounds check.


Synopsis
------------------------------------------------------------
1) .. code::

    template <input_iterator I, sentinel_for<I> S, class... Args>
    bool advance_if (I& first, S last, atomic_scannable_expression<Args...> e, Args&&... args)

Compares the first character in the range ``[first, last)`` with the expression ``e``. If additional arguments ``args`` are supplied and ``e`` is a predicate, they are forwarded to ``e``.




Returns
------------------------------------------------------------
:expr:`true` if the character satisfies the condition, :expr:`false` otherwise


Side effects
------------------------------------------------------------
Advances the first iterator of the range if the condition is satisfied. If the condition is not satisfied, no side effects occur.


Complexity
------------------------------------------------------------
1) Constant
2)



Notes
------------------------------------------------------------



Examples
------------------------------------------------------------

.. sourcecode::



Output

.. sourcecode::



.. seealso:: starts_with

