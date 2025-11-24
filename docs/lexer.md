# Lexer Documentation

The SynthFlow lexer is responsible for converting source code into a stream of tokens that can be processed by the parser.

## Token Types

The lexer recognizes the following token types:

### Keywords
- `fn` - Function declaration
- `let` - Variable declaration
- `if` - Conditional statement
- `else` - Alternative conditional branch
- `while` - Loop construct
- `return` - Return statement
- `type` - Type declaration
- `struct` - Structure declaration
- `enum` - Enumeration declaration
- `intent` - AI intent declaration

### Literals
- Integer literals (e.g., `42`, `0xFF`)
- Floating-point literals (e.g., `3.14`, `1.5e-3`)
- String literals (e.g., `"Hello, World!"`)
- Boolean literals (`true`, `false`)

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`
- Arrow: `->`

### Delimiters
- Parentheses: `(`, `)`
- Braces: `{`, `}`
- Brackets: `[`, `]`
- Separators: `,`, `.`, `:`, `;`

### Special Tokens
- `NEWLINE` - Line terminator
- `INDENT` - Indentation (for significant whitespace)
- `DEDENT` - Dedentation (for significant whitespace)
- `EOF_TOKEN` - End of file marker
- `INVALID` - Invalid token

## Implementation Details

The lexer is implemented in `compiler/src/lexer/lexer.cpp` and uses a recursive descent approach to tokenize the input source code.

### Key Methods

- `tokenize()` - Main entry point that converts source code to tokens
- `lexNumber()` - Handles numeric literals
- `lexIdentifier()` - Handles identifiers and keywords
- `lexString()` - Handles string literals
- `skipWhitespace()` - Skips whitespace characters
- `advance()` - Advances the position in the source code

## Usage

To use the lexer directly:

```cpp
#include "lexer.h"

std::string source = "fn main() -> Int { return 0; }";
Lexer lexer(source);
auto tokens = lexer.tokenize();
```

## Tests

Lexer tests are located in `compiler/tests/test_lexer.cpp` and can be run with the build scripts.