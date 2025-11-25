# Parser Implementation

The parser is responsible for transforming a sequence of tokens into an Abstract Syntax Tree (AST). It implements a recursive descent parser with operator precedence parsing for expressions.

## Parser Architecture

The parser follows a recursive descent approach where each grammar rule is implemented as a method in the [Parser](file:///D:/SynthFlow/synthflow/compiler/include/parser.h#L7-L38) class. The parser handles:

1. Expression parsing with correct precedence
2. Statement parsing
3. Error handling and reporting

## Grammar Rules

The parser implements the following grammar rules:

### Expressions
```
expression     → equality ;
equality       → comparison ( ( "==" | "!=" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" | "%" ) unary )* ;
unary          → ( "-" | "not" ) unary
               | primary ;
primary        → INTEGER | FLOAT | STRING | BOOLEAN
               | IDENTIFIER
               | "(" expression ")" ;
```

### Statements
```
statement      → expressionStmt
               | variableDecl
               | functionDecl
               | blockStmt
               | ifStmt
               | returnStmt ;

expressionStmt → expression ";"? ;
variableDecl   → "let" IDENTIFIER "=" expression ";"? ;
functionDecl   → "fn" IDENTIFIER "(" parameters? ")" blockStmt ;
blockStmt      → "{" statement* "}" ;
ifStmt         → "if" "(" expression ")" statement ( "else" statement )? ;
returnStmt     → "return" expression? ";"? ;
```

## Implementation Details

### Expression Parsing

The parser uses a Pratt parsing technique to handle operator precedence correctly. Each level of precedence is handled by a separate method:

1. [parseExpression()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L34-L36) - Entry point for expression parsing
2. [parseEquality()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L38-L48) - Handles `==` and `!=` operators
3. [parseComparison()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L50-L65) - Handles `<`, `>`, `<=`, `>=` operators
4. [parseTerm()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L67-L77) - Handles `+` and `-` operators
5. [parseFactor()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L79-L94) - Handles `*`, `/`, and `%` operators
6. [parseUnary()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L96-L107) - Handles unary operators like `-` and `not`
7. [parsePrimary()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L109-L144) - Handles literals, identifiers, and parenthesized expressions

### Statement Parsing

Statement parsing is handled by several methods:

1. [parseStatement()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L146-L163) - Main entry point for statement parsing
2. [parseExpressionStatement()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L165-L168) - Handles expression statements
3. [parseVariableDeclaration()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L170-L184) - Handles variable declarations with `let`
4. [parseFunctionDeclaration()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L186-L212) - Handles function declarations with `fn`
5. [parseBlockStatement()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L214-L228) - Handles block statements `{ ... }`
6. [parseIfStatement()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L230-L246) - Handles if statements
7. [parseReturnStatement()](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp#L248-L258) - Handles return statements

## Error Handling

The parser implements error handling through exceptions. When a syntax error is encountered, it throws a `std::runtime_error` with a descriptive message.

## Usage Example

```cpp
#include "lexer.h"
#include "parser.h"

// Tokenize source code
Lexer lexer(sourceCode);
auto tokens = lexer.tokenize();

// Parse tokens into AST
Parser parser(std::move(tokens));
auto statements = parser.parse();

// Process AST nodes...
```

## Testing

The parser is tested through comprehensive unit tests that verify:

1. Correct parsing of expressions with proper precedence
2. Correct parsing of various statement types
3. Proper error handling for malformed input
4. Round-trip parsing (parse and regenerate)

See [test_parser.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_parser.cpp) for implementation details.