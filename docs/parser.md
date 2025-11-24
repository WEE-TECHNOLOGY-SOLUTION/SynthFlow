# Parser Documentation

The SynthFlow parser is responsible for converting a stream of tokens into an Abstract Syntax Tree (AST) that represents the structure of the program.

## Parsing Process

The parser uses a recursive descent approach to convert tokens into AST nodes. It implements the following grammar rules:

### Expressions
- Primary expressions (literals, identifiers, parentheses)
- Unary expressions (negation, logical NOT)
- Multiplicative expressions (`*`, `/`, `%`)
- Additive expressions (`+`, `-`)
- Comparison expressions (`<`, `>`, `<=`, `>=`)
- Equality expressions (`==`, `!=`)
- Logical AND expressions (`&&`)
- Logical OR expressions (`||`)

### Statements
- Expression statements
- Variable declarations (`let`)
- Function declarations (`fn`)
- Block statements (`{ ... }`)
- If statements (`if ... else`)
- Return statements (`return`)

## Implementation Details

The parser is implemented in `compiler/include/parser.h` and `compiler/src/parser/parser.cpp`.

### Key Methods

- `parse()` - Main entry point that parses the entire program
- `parseExpression()` - Parses expressions with appropriate precedence
- `parseStatement()` - Parses individual statements
- `parsePrimary()` - Parses primary expressions (literals, identifiers)
- `match()` - Checks if the current token matches a specific type
- `advance()` - Consumes the current token and advances to the next

## Error Handling

The parser includes basic error handling for common syntax errors, such as mismatched parentheses or unexpected tokens.

## Usage

To use the parser:

```cpp
#include "parser.h"
#include "lexer.h"

// First tokenize the source code
std::string source = "fn main() -> Int { return 0; }";
Lexer lexer(source);
auto tokens = lexer.tokenize();

// Then parse the tokens into an AST
Parser parser(std::move(tokens));
auto ast = parser.parse();
```

## Future Extensions

Planned enhancements for the parser include:
- Support for more complex expressions
- Enhanced error reporting with line numbers and context
- Support for type annotations
- Better recovery from syntax errors