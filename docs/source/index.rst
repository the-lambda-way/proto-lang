.. comment
    For an automated documentation build solution, see
    https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

########################################################################################################################
Pattern Library
########################################################################################################################

.. toctree::
    :maxdepth: 2
    :caption: Contents:

########################################################################################################################
Indices and tables
########################################################################################################################

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`


########################################################################################################################
Documentation
########################################################################################################################

========================================================================================================================
Quick Look
========================================================================================================================

The Pattern Library is a toolkit for building recursive descent parsers, offering a range of features from low-level pointer manipulation, to high-level, grammar-like abstractions.

Here are various ways in which you might tokenize a number which can be either an integer or a decimal. Assume that the current character has been identified as a digit in your outer loop.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Set up
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     #include <string>
     #include <string_view>
     #include <variant>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     #include "syntax.h"

     enum class TokenType { INTEGER, DECIMAL };
     using number_token = token<TokenType, std::variant<int, double>>;

     constexpr bool is_digit (char c)     { return '0' <= c && c <= '9'; }

     std::string to_string (std::string_view s)     { return std::string {s.data(), s.length()}; }


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using pointers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     number_token number (scan_view& s)
     {
          // Integer
          std::string match = "" + *s++;

          while (is_digit(*s))    match += *s++;

          if (s[0] != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(match)};

          // Decimal
          match += *s++ + *s++;

          while (is_digit(*s))    match += *s++;

          return {TokenType::DECIMAL, std::stod(match)};
     }


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using algorithms
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     number_token number (scan_view& s)
     {
          s.save();

          // Integer
          ++s;
          advance_while(s, is_digit);

          if (*s != '.' || !is_digit(s[1]))    return {TokenType::INTEGER, std::stoi(s.copy_skipped())};

          // Decimal
          s += 2;
          advance_while(s, is_digit);

          return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
     }


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using higher-order functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     number_token number (scan_view& s)
     {
          constexpr auto integer    = Scan::some(is_digit);
          constexpr auto fractional = Scan::join('.', integer);

          s.save();

          integer(s);

          if (!fractional(s))     return {TokenType::INTEGER, std::stoi(s.copy_skipped())};
          return {TokenType::DECIMAL, std::stod(s.copy_skipped())};
     }


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using declarative features
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     auto integer    = Scan::some(is_digit);
     auto fractional = Scan::join('.', integer);

     number_token tokenize_int (std::string_view match)
     {
          return {TokenType::INTEGER, std::stoi(to_string(match))};
     };

     number_token tokenize_dec (std::string_view match)
     {
          return {TokenType::DECIMAL, std::stod(to_string(match))};
     };

     // Tokenize::incremental takes a list of pairs of scanners and tokenizers
     auto number = Tokenize::incremental({integer,    tokenize_int},
                                         {fractional, tokenize_dec});


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using grammar-like features
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code::

     auto digit   = GrammarExp >> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
     auto integer = +digit;
     auto decimal = integer + '.' + integer;

     number_token tokenize_int (std::string_view match)
     {
          return {TokenType::INTEGER, std::stoi(match)};
     };

     number_token tokenize_dec (std::string_view match)
     {
          return {TokenType::DECIMAL, std::stod(match)};
     };

     Language myLang;

     // Incremental parsing is handled automatically when rules are added
     myLang.add_rule(integer, tokenize_int);
     myLang.add_rule(decimal, tokenize_dec);


========================================================================================================================
Overview
========================================================================================================================

The Pattern Library is a toolkit for building recursive descent parsers, offering a range of features from low-level pointer manipulation, to high-level, grammar-like abstractions. Traditional parser generators embed semantic actions within a domain-specific parsing language. Instead, the Pattern Library is designed to embed parsers within normal C++ code. Thus, the custom needs of an application can be met using a mixture of high level and low level code as appropriate.

This library follows the principles of the C++ Standard Library, so that it fits naturally within a variety of program designs. Specifically, it avoids incidental runtime costs, prefers value semantics, uses generic algorithms where appropriate, and is compatible with the <algorithms> library, as well as ranges, views, and range adapters.

.. comment
     add to description
     

########################################################################################################################
Reference
########################################################################################################################

========================================================================================================================
Concepts
========================================================================================================================

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean_testable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Convertible to ``bool`` or usable in a boolean context. In the C++ Standard Library, this is still an exposition-only concept. It is incorporated here until the STL implements it.

.. code::

     template <class B>
     concept boolean_testable =
          std::convertible_to<B, bool> &&
          requires (B&& b) {
               { !std::forward<B>(b) } -> std::convertible_to<bool>;
          };


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean_invocable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A callable type which can be called with a set of arguments ``Args...`` and returns a boolean result. Unlike a predicate, invocation of the type is not required to be equality preserving, and its arguments are allowed to be modified.

.. code::

     template <class F, class... Args>
     concept boolean_invocable = std::invocable<F, Args...> &&
                                 boolean_testable<std::invoke_result_t<F, Args...>>;


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
indirectly_equality_comparable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Two iterators are indirectly equality comparable if the types they reference are equality comparable.

.. code::

     template <class I1, class I2, class P1 = std::identity, class P2 = std::identity>
     concept indirectly_equality_comparable = std::indirectly_comparable<I1, I2, std::ranges::equal_to, P1, P2>;


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
range_equality_comparable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Two ranges are equality comparable if their iterators are indirectly equality comparable.

.. code::

     template <class R1, class R2, class P1 = std::identity, class P2 = std::identity>
     concept range_equality_comparable =
          std::ranges::range<R1> && std::ranges::range<R2> &&
          indirectly_equality_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>, P1, P2>;


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
scanning_algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Takes a reference to an iterator and a sentinel, and returns a boolean-testable result. The iterator is required to be multipass to support speculative testing. Normatively, the callable type should advance the iterator if its internal condition is satisfied, and otherwise leave it in its original state.

.. code::

     template <class F, class I, class S>
     concept scanning_algorithm = std::forward_iterator<I> && std::sentinel_for<S, I> && boolean_invocable<F, I&, S>;


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mutable_forward_range
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A refinement of ``std::ranges::forward_range`` that provides a references to its iterator.

.. code::

     template <class R>
     concept mutable_forward_range = std::ranges::forward_range<R> && std::is_reference_v<std::ranges::iterator_t<R>>;


========================================================================================================================
Function Combinators
========================================================================================================================
Facilities for declaratively composing functions.

Base algorithms are found in the namespace ``fn``.

Each algorithm in ``fn`` has a counterpart in the namespace ``fo`` which binds the child function, and any parameters of the algorithm, to a function object which defines the function call operator to forwards its arguments to the bound function. These functions are known colloquially as combinators.

User-defined functions passed to combinators should implement a predicate on its arguments, possibly mutating them. When composing impure functions, care must be taken by the user to properly manage state when an algorithm fails midway.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::bind_back
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Binds arguments to the back of a function, using the same semantics as ``std::bind_front``.


Synopsis
------------------------------------------------------------
1) .. code::

     template <typename F, typename... Args>
     constexpr /*unspecified*/ bind_back (F&& f, Args&&... args)

Generates a forwarding call wrapper for ``f``. Invoking ``fn::bind_back(f, bound_args...)(call_args...)`` is equivalent to ``std::invoke(f, call_args..., bound_args...)``.

The program is ill-formed if any of the following is ``false``:

* ``std::is_constructible_v<std::decay_t<F>, F>``
* ``std::is_move_constructible_v<std::decay_t<F>>``
* ``(std::is_constructible_v<std::decay_t<Args>, Args> && ...)``
* ``(std::is_move_constructible_v<std::decay_t<Args>> && ...)``


Returns
------------------------------------------------------------
A function object of a type that is unspecified, except that the types of objects returned by two calls to ``fn::bind_back`` with the same arguments are the same.


Exceptions
------------------------------------------------------------
Only throws if construction of stored function object or any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
One invocation of ``f``.


Notes
------------------------------------------------------------
As described in ``std::invoke``, when invoking a pointer to non-static member function or pointer to non-static data member, the first argument has to be a reference or pointer (including, possibly, smart pointer such as ``std::shared_ptr`` and ``std::unique_ptr``) to an object whose member will be accessed.

The bound arguments to ``fn::bind_back`` are copied or moved, and are never passed by reference unless wrapped in ``std::ref`` or ``std::cref``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     #include "fn-combinators.h"
     using namespace Pattern;
     using namespace std::literals::string_view_literals;

     int main
     {
          scan_view s = "Typing is no substitute for thinking. -- Richard W. Hamming"

          auto scan_first_word = fn::bind_back(scan, "Typing "sv);
          scan_first_word(s);

          std::cout << "Next letter: " << *s << '\n';
     }

Output

.. code::

     Next letter: i



~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::identity, fo::identity
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::identity =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` and returns its result.

2) .. code::

     /*unspecified*/ fo::identity (auto&& f)

Binds ``f`` to a function object which calls ``fn::identity`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
The result of invoking ``f`` with ``args...``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
One invocation of ``f``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          bool r1 = fn::identity(scan, s1, "Hello, "sv);
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::identity(fn::bind_back(scan, "Hello, "sv));
          bool r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1: W
     r1: true
     s2: W
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::negate, fo::negate
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::negate =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool


Invokes ``f`` with ``args...`` and negates the result.

2) .. code::

     /*unspecified*/ fo::negate (auto&& f)

Binds ``f`` to a function object which calls ``fn::negate`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
The negation of the result of invoking ``f`` with ``args...``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
One invocation of ``f``.


Notes
------------------------------------------------------------
Negation is suitable for pure functions, where no state changes occur on a positive result. Impure functions should create their own negation counterpart which properly manages its internal state under a failure condition. This might involve, for example, restoring data to a previous state.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          auto r1 = fn::negate(scan, s1, "Hello, "sv);
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::negate(fn::bind_back(scan, "Hello, "sv));
          auto r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1: W
     r1: false
     s2: W
     r2: false


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::optional, fo::optional
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::optional =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` and always returns true.

2) .. code::

     /*unspecified*/ fo::optional (auto&& f)

Binds ``f`` to a function object which calls ``fn::optional`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
One invocation of ``f``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          bool r1 = fn::optional(scan, s1, 'z');
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::optional(fn::bind_back(scan, 'z'));
          bool r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1: H
     r1: true
     s2: H
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::at_most, fo::at_most
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::at_most =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` up to ``n`` times or failure and always returns true.

2) .. code::

     /*unspecified*/ fo::at_most (std::size_t n, auto&& f)

Binds ``n`` and ``f`` to a function object which calls ``fn::at_most`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
Up to ``n`` invocations of ``f``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::at_most(3, scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::at_most(3, fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 3
     r1: true
     s2 count: 3
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::n_times, fo::n_times
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::n_times =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` exactly ``n`` times, stopping early on failure, and returning true if all ``n`` invocations succeed.

2) .. code::

     /*unspecified*/ fo::n_times (std::size_t n, auto&& f)

Binds ``n`` and ``f`` to a function object which calls ``fn::n_times`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true`` if all ``n`` invocations succeed, else ``false``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
Exactly ``n`` invocations of ``f``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::n_times(3, scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::n_times(3, fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 3
     r1: true
     s2 count: 3
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::repeat, fo::repeat
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::repeat =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t min, std::size_t max, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args..`` at least ``min`` times and at most ``max`` times, returning true if at least ``min`` invocations succeeds.

2) .. code::

     /*unspecified*/ fo::repeat (std::size_t min, std::size_t max, auto&& f)

Binds ``min``, ``max``, and ``f`` to a function object which calls ``fn::repeat`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true`` if at least ``min`` invocations succeeds.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
Between ``min`` and ``max`` invocations of ``f``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::repeat(3, 4, scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::repeat(3, 4, fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 4
     r1: true
     s2 count: 4
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::many, fo::many
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::many =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` until failure and always returns true.

2) .. code::

     /*unspecified*/ fo::many (auto&& f)

Binds ``f`` to a function object which calls ``fn::many`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true``


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
The number of successful invocations of ``f``.


Notes
------------------------------------------------------------
A child function which never fails will result in an infinite loop. For instance, it might be unwise to pass a ``fn::optional`` composition directly to ``fn::many``, since there is no way of breaking out of the loop, aside from exiting the program.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::many(scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::many(fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 5
     r1: true
     s2 count: 5
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::at_least, fo::at_least
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::at_least =
     []
     <class... Args, boolean_invocable<Args...> F>
     (size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args..`` at least ``n`` times until failure, returning true if at least ``n`` invocations succeed.

2) .. code::

     /*unspecified*/ fo::at_least (std::size_t n, auto&& f)

Binds ``n`` and ``f`` to a function object which calls ``fn::at_least`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true`` if at least ``n`` invocations of ``f`` succeed.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
At least ``n`` invocations of ``f``.


Notes
------------------------------------------------------------
A child function which never fails will result in an infinite loop. For instance, it might be unwise to pass a ``fn::optional`` composition directly to ``fn::at_least``, since there is no way of breaking out of the loop, aside from exiting the program.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::least(3, scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::least(3, fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 5
     r1: true
     s2 count: 5
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::some, fo::some
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     auto fn::some =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` until failure, and returns true if at least one invocation succeeds.

2) .. code::

     /*unspecified*/ fo::some (auto&& f)

Binds ``f`` to a function object which calls ``fn::some`` when invoked, forwarding any arguments passed.


Returns
------------------------------------------------------------
``true`` if at least one invocation of ``f`` succeeds.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the call to ``f`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
The number of successful invocations of ``f``.


Notes
------------------------------------------------------------
A child function which never fails will result in an infinite loop. For instance, it might be unwise to pass a ``fn::optional`` composition directly to ``fn::some``, since there is no way of breaking out of the loop, aside from exiting the program.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello? Hello? Hello? Hello? Hello? ";
          scan_view s1 {source};
          scan_view s2 {source};

          bool r1 = fn::some(scan, s1, "Hello? "sv);
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::some(fn::bind_back(scan, "Hello? "sv));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1 count: 5
     r1: true
     s2 count: 5
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::any, fo::any
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     bool fn::any (boolean_invocable auto&&... f)

Invokes a set of functions until one returns true. Returns false if none succeed.

2) .. code::

     template <class... Args, boolean_invocable<Args...>... F>
     bool fn::any (F&&... f, std::tuple<Args...>&& args)

Invokes a set of functions until one returns true, passing each function the elements of ``args``. Returns false if none succeed.

3) .. code::

     /*unspecified*/ fo::any (auto&&... f)

Binds ``f...`` to a function object. When invoked with a set of arguments ``args...``, invokes each function until one returns true, passing each function ``args...``. Returns false if none succeed.


Returns
------------------------------------------------------------
``true`` when the first function in ``f...`` succeeds, or ``false`` if none do.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the calls to ``f...`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
The sum of the invocations of functions from the set ``f...``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello, World!";
          scan_view s1 {source};
          scan_view s2 {source};

          auto f1 = fn::bind_back(scan, "Hello, "sv);
          auto f2 = fn::bind_back(scan, "World"sv);

          bool r1 = fn::any(f1, f2, {s1});
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto g = fo::any(f1, f2);
          bool r1 = g(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1: W
     r1: true
     s2: W
     r2: true


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fn::all, fo::all
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Synopsis
------------------------------------------------------------
1) .. code::

     bool fn::all (boolean_invocable auto&&... f)

Invokes a set of functions while they return true. Returns false if any failed.

2) .. code::

     template <class... Args, boolean_invocable<Args...>... F>
     bool fn::all (F&&... f, std::tuple<Args...>&& args)

Invokes a set of functions while they return true, passing each the elements of ``args``. Returns false if any failed.

3) .. code::

     /*unspecified*/ fo::all (auto&&... f)

Binds ``f...`` to a function object. When invoked with a set of arguments ``args...``, invokes each function while they return true, passing each function ``args...``. Returns false if any failed.


Returns
------------------------------------------------------------
``true`` if all functions in the set ``f...`` succeed, else ``false``.


Exceptions
------------------------------------------------------------
Only throws if the called child function, or construction of any of the bound arguments throws.


Side effects
------------------------------------------------------------
Any side effects of the calls to ``f...`` or of the construction of ``args...``.


Complexity
------------------------------------------------------------
The sum of the invocations of functions from the set ``f...``.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          std::string_view source = "Hello, World!";
          scan_view s1 {source};
          scan_view s2 {source};

          auto f1 = fn::bind_back(scan, "Hello, "sv);
          auto f2 = fn::bind_back(scan, "World"sv);

          bool r1 = fn::all(f1, f2, {s1});
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto g = fo::all(f1, f2);
          bool r1 = g(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code::

     s1: !
     r1: true
     s2: !
     r2: true


========================================================================================================================
Scanning Functions
========================================================================================================================

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
scan, scan_if
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Given a mutable range, advances its iterator if its dereferenced value satisfies a condition.


Synopsis
------------------------------------------------------------
1) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::equality_comparable_with<std::iter_value_t<I>> E>
     bool scan (I& first, S last, E element)

Advances ``first`` by one if its dereferenced value equals ``element``.

2) .. code::

     template <mutable_forward_range R,
               std::equality_comparable_with<std::ranges::iterator_t<R>> E>
     bool scan (R&& r, E&& element)

Same as (1), using ``r`` as the source range.

3) .. code::

     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool scan (I1& first1, S1 last1, I2 first2, S2 last2)

Advances ``first1`` if ``[first1, last1)`` begins with the elements of ``[first2, last2)``. After a successful match, ``first1`` has been advanced by the size of ``[first2, last2)``.

4) .. code::

     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan (R&& r, I&& first, S&& last)

Same as (3), using ``r`` as the source range.

5) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan (I&& first, S&& last, R&& r)

Same as (3), using ``r`` as the comparison range.

6) .. code::

     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool scan (R1&& r1, R2&& r2)

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool scan_if (I& first, S last, P pred)

Advances ``first`` by one if ``std::invoke(pred, *first)`` returns true;

8) .. code::

     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::range_value_t<R>> P>
     bool scan_if (R&& r, P&& pred)

Same as (7), using ``r`` as the source range.


Returns
------------------------------------------------------------
``true`` if the condition was satisfied, ``false`` otherwise.


Side effects
------------------------------------------------------------
May advance the iterator of the source range one or more times, using the increment operator.


Complexity
------------------------------------------------------------
| 1, 2) O(1)
| 3-6) O(n), where n is the number of elements in ``[first2, last2)``
| 7, 8) O(1) + one invocation of ``pred``


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view source = "Programs must be written for people to read, "
                             "and only incidentally for machines to execute. "
                             "-- Harold Abelson";

          scan(s, 'P');
          std::cout << "Single element: " << *s << '\n';

          scan(s, "rograms m");
          std::cout << "Range of elements: " << *s << '\n';

          scan_if(s, [](char c) { return c == 'u'; });
          std::cout << "Predicate: " << *s << '\n';
     }

Output

.. code::

     Single element: r
     Range of elements: u
     Predicate: s


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
scan_not, scan_if_not
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Given a mutable range, advances its iterator by one if its dereferenced value fails to satisfy a condition.


Synopsis
------------------------------------------------------------
1) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::equality_comparable_with<std::iter_value_t<I>> E>
     bool scan_not (I& first, S last, E element)

Advances ``first`` by one if its dereferenced value doesn't equal ``element``.

2) .. code::

     template <mutable_forward_range R,
               std::equality_comparable_with<std::ranges::iterator_t<R>> E>
     bool scan_not (R&& r, E&& element)

Same as (1), using ``r`` as the source range.

3) .. code::

     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
               std::forward_iterator I2, std::sentinel_for<I2> S2>
          requires indirectly_equality_comparable<I1, I2>
     bool scan_not (I1& first1, S1 last1, I2 first2, S2 last2)

Advances ``first1`` by one if ``[first1, last1)`` doesn't begin with the elements of ``[first2, last2)``.

4) .. code::

     template <mutable_forward_range R, std::forward_iterator I, std::sentinel_for<I> S>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan_not (R&& r, I&& first, S&& last)

Same as (3), using ``r`` as the source range.

5) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R>
          requires indirectly_equality_comparable<std::ranges::iterator_t<R>, I>
     bool scan_not (I&& first, S&& last, R&& r)

Same as (3), using ``r`` as the comparison range.

6) .. code::

     template <mutable_forward_range R1, std::ranges::input_range R2>
          requires range_equality_comparable<R1, R2>
     bool scan_not (R1&& r1, R2&& r2)

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               std::indirect_unary_predicate<I> P>
     bool scan_if_not (I& first, S last, P pred)

Advances ``first`` by one if ``std::invoke(pred, *first)`` returns ``false``.

8) .. code::

     template <mutable_forward_range R,
               std::indirect_unary_predicate<std::ranges::range_value_t<R>> P>
     bool scan_if_not (R&& r, P&& pred)

Same as (7), using ``r`` as the source range.

9) .. code::

     template <std::forward_iterator I, std::sentinel_for<I> S,
               scanning_algorithm<I, S> F>
     bool scan_if_not (I& first, S last, F f)

Advances ``first`` by one if ``std::invoke(f, copy, last)`` returns ``false``, where ``copy`` is a copy of ``first``.

10) .. code::

     template <mutable_forward_range R,
               scanning_algorithm<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> F>
     bool scan_if_not (R&& r, F&& f)

Same as (9), using ``r`` as the source range.


Returns
------------------------------------------------------------
``true`` if the condition was not satisfied, ``false`` otherwise.


Side effects
------------------------------------------------------------
May advance the iterator of the source range one time, using the increment operator.


Complexity
------------------------------------------------------------
| 1, 2) O(1)
| 3-6) O(n), where n is the number of elements in ``[first2, last2)``
| 7-10) O(1) + one invocation of ``pred`` or ``f``


Notes
------------------------------------------------------------
``scan_not`` and ``scan_if_not`` are implemented as function objects ``scan_not_t`` and ``scan_if_not_t``, so they can be passed conveniently to other functions.


Examples
------------------------------------------------------------

.. code::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view source = "Talk is cheap. Show me the code. -- Linus Torvalds";

          scan_not(s, 'Q');
          std::cout << "Single element: " << *s << '\n';

          scan_not(s, "alks");
          std::cout << "Range of elements: " << *s << '\n';

          auto equals_foo = [](auto& s) { return scan(s, "foo"); };
          scan_if_not(s, equals_foo);
          std::cout << "Scanning algorithm: " << *s << '\n';
     }

Output

.. code::

     Single element: a
     Range of elements: l
     Scanning algorithm: k
