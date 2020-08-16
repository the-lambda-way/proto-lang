************************************************************************************************************************
Concepts
************************************************************************************************************************

========================================================================================================================
boolean_testable
========================================================================================================================
Convertible to ``bool`` or usable in a boolean context. In the C++ Standard Library, this is still an exposition-only concept. It is incorporated here until the STL implements it.

.. code-block::

     template <class B>
     concept boolean_testable =
          std::convertible_to<B, bool> &&
          requires (B&& b) {
               { !std::forward<B>(b) } -> std::convertible_to<bool>;
          };


========================================================================================================================
boolean_invocable
========================================================================================================================
A callable type which can be called with a set of arguments ``Args...`` and returns a boolean result. Unlike a predicate, invocation of the type is not required to be equality preserving, and its arguments are allowed to be modified.

.. code-block::

     template <class F, class... Args>
     concept boolean_invocable = std::invocable<F, Args...> &&
                                 boolean_testable<std::invoke_result_t<F, Args...>>;


========================================================================================================================
indirectly_equality_comparable
========================================================================================================================
Two iterators are indirectly equality comparable if the types they reference are equality comparable.

.. code-block::

     template <class I1, class I2, class P1 = std::identity, class P2 = std::identity>
     concept indirectly_equality_comparable = std::indirectly_comparable<I1, I2, std::ranges::equal_to, P1, P2>;


========================================================================================================================
range_equality_comparable
========================================================================================================================
Two ranges are equality comparable if their iterators are indirectly equality comparable.

.. code-block::

     template <class R1, class R2, class P1 = std::identity, class P2 = std::identity>
     concept range_equality_comparable =
          std::ranges::range<R1> && std::ranges::range<R2> &&
          indirectly_equality_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>, P1, P2>;


========================================================================================================================
mutable_forward_range
========================================================================================================================
A refinement of ``std::ranges::forward_range`` that provides a reference to its iterator.

.. code-block::

     template <class R>
     concept mutable_forward_range = std::ranges::forward_range<R> && std::is_reference_v<std::ranges::iterator_t<R>>;
