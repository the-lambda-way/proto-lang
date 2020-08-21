************************************************************************************************************************
Function Combinators
************************************************************************************************************************
Facilities for declaratively composing functions.

Base algorithms are found in the namespace ``fn``.

Each algorithm in ``fn`` has a counterpart in the namespace ``fo`` which binds the child function, and any parameters of the algorithm, to a function object which defines the function call operator to forward its called arguments to the bound function. These objects are known colloquially as combinators.

User-defined functions passed to combinators should implement a predicate on its arguments, possibly mutating them. When composing impure functions, care must be taken by the user to properly manage state when an algorithm fails midway.


========================================================================================================================
fn::bind_back
========================================================================================================================
Binds arguments to the back of a function, using the same semantics as ``std::bind_front``.


Synopsis
------------------------------------------------------------
1) .. code-block::

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

.. code-block::

     #include <iostream>
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     #include "fn-combinators.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s = "Typing is no substitute for thinking. -- Richard W. Hamming";

          auto scan_first_word = fn::bind_back(scan, "Typing ");
          scan_first_word(s);

          std::cout << "Next letter: " << *s << '\n';
     }

Output

.. code-block:: text

     Next letter: i



========================================================================================================================
fn::identity, fo::identity
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::identity =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` and returns its result.

2) .. code-block::

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

.. code-block::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          std::cout << std::boolalpha;

          bool r1 = fn::identity(scan, s1, "Hello, ");
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::identity(fn::bind_back(scan, "Hello, "));
          bool r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code-block:: text

     s1: W
     r1: true
     s2: W
     r2: true


========================================================================================================================
fn::negate, fo::negate
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::negate =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool


Invokes ``f`` with ``args...`` and negates the result.

2) .. code-block::

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

.. code-block::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          std::cout << std::boolalpha;

          auto r1 = fn::negate(scan, s1, "Hello, ");
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::negate(fn::bind_back(scan, "Hello, "));
          auto r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code-block:: text

     s1: W
     r1: false
     s2: W
     r2: false


========================================================================================================================
fn::optional, fo::optional
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::optional =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...``, and always returns true.

2) .. code-block::

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

.. code-block::

     #include <iostream>
     #include "fn-combinators.h"
     #include "scan_view.h"
     #include "scanning-algorithms.h"
     using namespace Pattern;

     int main ()
     {
          scan_view s1 = "Hello, World!";
          scan_view s2 = s1;

          std::cout << std::boolalpha;

          bool r1 = fn::optional(scan, s1, 'z');
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto f = fo::optional(fn::bind_back(scan, 'z'));
          bool r2 = f(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code-block:: text

     s1: H
     r1: true
     s2: H
     r2: true


========================================================================================================================
fn::at_most, fo::at_most
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::at_most =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` up to ``n`` times or failure, and always returns true.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::at_most(3, scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::at_most(3, fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 3
     r1:       true
     s2 count: 3
     r2:       true


========================================================================================================================
fn::n_times, fo::n_times
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::n_times =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` exactly ``n`` times, stopping early on failure, and returning true if all ``n`` invocations succeed.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::n_times(3, scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::n_times(3, fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 3
     r1:       true
     s2 count: 3
     r2:       true


========================================================================================================================
fn::repeat, fo::repeat
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::repeat =
     []
     <class... Args, boolean_invocable<Args...> F>
     (std::size_t min, std::size_t max, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args..`` at least ``min`` times and at most ``max`` times, returning true if at least ``min`` invocations succeeds.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::repeat(3, 4, scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::repeat(3, 4, fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 4
     r1:       true
     s2 count: 4
     r2:       true


========================================================================================================================
fn::many, fo::many
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::many =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` until failure and always returns true.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::many(scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::many(fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 5
     r1:       true
     s2 count: 5
     r2:       true


========================================================================================================================
fn::at_least, fo::at_least
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::at_least =
     []
     <class... Args, boolean_invocable<Args...> F>
     (size_t n, F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args..`` at least ``n`` times until failure, returning true if at least ``n`` invocations succeed.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::at_least(3, scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::at_least(3, fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 5
     r1:       true
     s2 count: 5
     r2:       true


========================================================================================================================
fn::some, fo::some
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     auto fn::some =
     []
     <class... Args, boolean_invocable<Args...> F>
     (F&& f, Args&&... args) -> bool

Invokes ``f`` with ``args...`` until failure, and returns true if at least one invocation succeeds.

2) .. code-block::

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

.. code-block::

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

          std::cout << std::boolalpha;

          bool r1 = fn::some(scan, s1, "Hello? ");
          std::cout << "s1 count: " << (s1 - source.begin()) / 7 << '\n';
          std::cout << "r1:       " << r1 << '\n';

          auto f = fo::some(fn::bind_back(scan, "Hello? "));
          bool r2 = f(s2);
          std::cout << "s2 count: " << (s2 - source.begin()) / 7 << '\n';
          std::cout << "r2:       " << r2 << '\n';
     }

Output

.. code-block:: text

     s1 count: 5
     r1:       true
     s2 count: 5
     r2:       true


========================================================================================================================
fn::any, fo::any
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     bool fn::any (boolean_invocable auto&&... f)

Invokes a set of functions until one returns true. Returns false if none succeed.

2) .. code-block::

     template <class... Args, boolean_invocable<Args...>... F>
     bool fn::any (std::tuple<Args...>&& args, F&&... f)

Invokes a set of functions until one returns true, passing each function the elements of ``args``. Returns false if none succeed.

3) .. code-block::

     /*unspecified*/ fo::any (auto&&... f)

Binds ``f...`` to a function object. When invoked with a set of arguments ``call_args...``, invokes each function until one returns true, passing each function ``call_args...``. Returns false if none succeed.


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

.. code-block::

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

          auto f1 = fn::bind_back(scan, "Hello, ");
          auto f2 = fn::bind_back(scan, "World");

          std::cout << std::boolalpha;

          bool r1 = fn::any(std::make_tuple(std::ref(s1)), f1, f2);
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto g = fo::any(f1, f2);
          bool r2 = g(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code-block:: text

     s1: W
     r1: true
     s2: W
     r2: true


========================================================================================================================
fn::all, fo::all
========================================================================================================================

Synopsis
------------------------------------------------------------
1) .. code-block::

     bool fn::all (boolean_invocable auto&&... f)

Invokes a set of functions while they return true. Returns false if any failed.

2) .. code-block::

     template <class... Args, boolean_invocable<Args...>... F>
     bool fn::all (std::tuple<Args...>&& args, F&&... f)

Invokes a set of functions while they return true, passing each the elements of ``args``. Returns false if any failed.

3) .. code-block::

     /*unspecified*/ fo::all (auto&&... f)

Binds ``f...`` to a function object. When invoked with a set of arguments ``call_args...``, invokes each function while they return true, passing each function ``call_args...``. Returns false if any failed.


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

.. code-block::

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

          auto f1 = fn::bind_back(scan, "Hello, ");
          auto f2 = fn::bind_back(scan, "World");

          std::cout << std::boolalpha;

          bool r1 = fn::all(std::make_tuple(std::ref(s1)), f1, f2);
          std::cout << "s1: " << *s1 << '\n';
          std::cout << "r1: " << r1 << '\n';

          auto g = fo::all(f1, f2);
          bool r2 = g(s2);
          std::cout << "s2: " << *s2 << '\n';
          std::cout << "r2: " << r2 << '\n';
     }

Output

.. code-block:: text

     s1: !
     r1: true
     s2: !
     r2: true
