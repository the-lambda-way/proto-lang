# Proto Language
(Note: this language is still in its initial stage, with various techniques being explored.)

Proto is on a mission to decentralize programming from the monolithic languages we currently are forced to operate within. Too much attention is spent on mastering the idiosyncracies and constraints of existing languages, when we should be focused solely on the considerations of the domains under consideration.

Proto divides this problem into two aspects: syntax conventions and execution models.

I propose the radical view that syntax should not defined for us, but that conventions should arise naturally out of common usage, just as they do within science, engineering, and in natural language. This is already the situation in API design - Proto would simply extend this to syntax usage as well. Language-oriented programming environments like Racket and JetBrains MPS currently have a lot to offer in this area, but I think the situation can be pushed even further. Working with syntax extensions should be as easy as working with functions or objects. I consider Racket to be the forerunner in this area, but it is currently anchored to its lisp roots. A newly announced effort by Matthew Flatt, dubbed Racket2 (<https://github.com/racket/racket/wiki/Racket2>), is a step in this direction, and worth keeping an eye on.

Execution models govern both the capabilities of a programming language, as well as the semantics which are most convenient to use. It is harder to envision how to put control of these in the hands of language users in an accessible way, but for a start I think it is worth exposing a representative set of small components with formally-defined semantics, that might be reflective of a compiler-building toolbox with a strong emphasis on compositional design. Ideally, it would be possible to move between functional semantics and stateful semantics (for example) in the same program along well-defined boundaries, without incidental performance penalty or complexity, and without relying on ad-hoc language integration. The same way that Racket users can declare a #lang to work with, Proto users could declare a paradigm or execution environment.

If you feel like chatting, feel free to stop on by the Discord server (https://discord.gg/KxD2U4f).
