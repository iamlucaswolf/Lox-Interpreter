# The Lox Programming Language (Interpreter)

This repository implements a _tree-walk_ interpreter for the Lox programming
language, originally invented by [Robert Nystorm](https://stuffwithstuff.com).
The interpreter follows the design mapped out in Nystorm's excellent book 
[Crafting Interpreters](https://craftinginterpreters.com), yet the implemention
is **written in C++ (rather than Java)**.

_This work is currently in progress. I will update the README as the project 
grows._

## Design

The project is organized into several independent components:

- [Scanner](#Scanner)
- to be continued...

### Scanner
Source: [`scanner.h`](scanner.h)

The scanner dissects Lox source code into a list of syntactical tokens. 
Contrary to the book's approach, this scanner is implemented as a single 
function `vector<Token> scan(const string &int)`, rather than a `Scanner` class.

This decision has been made, since scanning is conceptually a function 
(i.e. a deterministic mapping from source code to Token `vector`s) and also to
ensure that scanning is an 
[idempotent](https://en.wikipedia.org/wiki/Idempotence) operation. Curiously, 
this isn't the case in Nystorm's implementation:

```java
// Implementation from "Crafting Interpreters"

String source = getSource(...);
Scanner scanner = new Scanner(source);

List<Token> tokens = scanner.scan(); // yields expected list of tokens

// Calling scanner.scan() a second time, perhaps accidentally
tokens = scanner.scan(); // yields an empty list
```

```cpp
// This implementation

string source { get_source(...) };

vector<Token> tokens { scan(source) }; // yields expected list of tokens

// Calling scan() a second time, perhaps accidentally
tokens = scan(source); // yields expected list of tokens; scan() is idempotent
```

Nevertheless, `scan()` still relies on  a `Scanner` class similar to Nystorm's .
This could have probably been refactored into a fully-fledged NFA/DFA-based 
implementation, however I ultimately decided against that in order to not 
over-engineer this rather trivial part of the project.
