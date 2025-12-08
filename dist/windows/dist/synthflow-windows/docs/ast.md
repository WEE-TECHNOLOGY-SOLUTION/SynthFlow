# Abstract Syntax Tree (AST) Documentation

The Abstract Syntax Tree (AST) is a tree representation of the source code that is used by the SynthFlow compiler for semantic analysis and code generation.

## AST Node Hierarchy

### Base Classes
- `ASTNode` - Base class for all AST nodes
- `Expression` - Base class for all expression nodes
- `Statement` - Base class for all statement nodes

### Expressions

#### Literals
- `IntegerLiteral` - Integer values (e.g., `42`)
- `FloatLiteral` - Floating-point values (e.g., `3.14`)
- `StringLiteral` - String values (e.g., `"Hello"`)
- `BooleanLiteral` - Boolean values (`true`, `false`)
- `Identifier` - Variable and function names

#### Operations
- `BinaryOp` - Binary operations (e.g., `a + b`, `x > y`)

### Statements
- `ExpressionStatement` - Expression as a statement
- `VariableDeclaration` - Variable declarations (`let x = 5`)
- `FunctionDeclaration` - Function declarations (`fn foo() { }`)
- `BlockStatement` - Block of statements (`{ stmt1; stmt2; }`)
- `IfStatement` - Conditional statements (`if cond { } else { }`)
- `ReturnStatement` - Return statements (`return value`)

## Visitor Pattern

The AST uses the visitor pattern for traversal and manipulation. All AST nodes have an `accept` method that takes an `ASTVisitor` instance.

### ASTVisitor Interface
The `ASTVisitor` class defines visit methods for each concrete AST node type, allowing for extensible operations on the AST.

## Implementation Details

The AST is implemented in `compiler/include/ast.h` and uses smart pointers (`std::unique_ptr`) for memory management.

## Usage Example

```cpp
// Creating an integer literal
auto intLiteral = std::make_unique<IntegerLiteral>(42);

// Creating a binary operation (5 + 3)
auto left = std::make_unique<IntegerLiteral>(5);
auto right = std::make_unique<IntegerLiteral>(3);
auto binOp = std::make_unique<BinaryOp>(std::move(left), BinaryOp::ADD, std::move(right));
```