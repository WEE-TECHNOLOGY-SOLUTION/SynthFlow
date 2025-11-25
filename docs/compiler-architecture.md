# Compiler Architecture

The SynthFlow compiler is structured as a multi-phase compiler that transforms source code into executable code through several distinct stages.

## Compiler Phases

### 1. Lexical Analysis (Tokenization)

The lexer ([Lexer](file:///D:/SynthFlow/synthflow/compiler/include/lexer.h#L7-L30)) is responsible for converting the source code into a stream of tokens. It handles:

- Keyword recognition
- Identifier and literal parsing
- Operator and delimiter identification
- Whitespace and comment handling
- Error reporting for invalid characters

See [lexer.md](file:///D:/SynthFlow/synthflow/docs/lexer.md) for detailed documentation.

### 2. Syntax Analysis (Parsing)

The parser ([Parser](file:///D:/SynthFlow/synthflow/compiler/include/parser.h#L7-L38)) transforms the token stream into an Abstract Syntax Tree (AST). It implements:

- Recursive descent parsing
- Operator precedence handling
- Grammar rule enforcement
- Syntax error detection and reporting

See [parser.md](file:///D:/SynthFlow/synthflow/docs/parser.md) for detailed documentation.

### 3. Semantic Analysis

The semantic analyzer ([SemanticAnalyzer](file:///D:/SynthFlow/synthflow/compiler/include/semantic_analyzer.h#L6-L41)) checks the semantic correctness of the AST. It performs:

- Symbol table management
- Type checking
- Scope analysis
- Declaration validation

See [semantic-analyzer.md](file:///D:/SynthFlow/synthflow/docs/semantic-analyzer.md) for detailed documentation.

### 4. Code Generation (Planned)

The code generator will transform the validated AST into executable code or bytecode.

## Directory Structure

```
compiler/
├── include/           # Header files
├── src/               # Source files
│   ├── lexer/         # Lexer implementation
│   ├── parser/        # Parser implementation
│   ├── ast/           # AST visitor implementations
│   ├── semantic/      # Semantic analyzer implementation
│   └── codegen/       # Code generation (planned)
└── tests/             # Unit tests
```

## Data Flow

1. Source code is read from a file
2. [Lexer](file:///D:/SynthFlow/synthflow/compiler/include/lexer.h#L7-L30) tokenizes the source into a vector of [Token](file:///D:/SynthFlow/synthflow/compiler/include/token.h#L26-L35) objects
3. [Parser](file:///D:/SynthFlow/synthflow/compiler/include/parser.h#L7-L38) converts tokens into an AST of [Statement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L68-L68) and [Expression](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L17-L17) nodes
4. [SemanticAnalyzer](file:///D:/SynthFlow/synthflow/compiler/include/semantic_analyzer.h#L6-L41) validates the AST for semantic correctness
5. Code generator (future phase) produces executable code

## AST Node Hierarchy

### Expressions
- [Expression](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L17-L17) (Base class)
  - [IntegerLiteral](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L19-L24)
  - [FloatLiteral](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L26-L31)
  - [StringLiteral](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L33-L38)
  - [BooleanLiteral](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L40-L45)
  - [Identifier](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L47-L52)
  - [BinaryOp](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L55-L65)

### Statements
- [Statement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L68-L68) (Base class)
  - [ExpressionStatement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L70-L76)
  - [VariableDeclaration](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L78-L86)
  - [FunctionDeclaration](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L88-L98)
  - [BlockStatement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L100-L108)
  - [IfStatement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L110-L120)
  - [ReturnStatement](file:///D:/SynthFlow/synthflow/compiler/include/ast.h#L122-L129)

## Visitor Pattern

The AST uses the visitor pattern to enable traversal and processing of nodes. Each node implements an `accept` method that calls the appropriate `visit` method on the visitor.

## Error Handling

Each phase implements appropriate error handling:

- Lexical errors for invalid characters
- Syntax errors for malformed code
- Semantic errors for type mismatches and undeclared identifiers

## Build System

The compiler uses a Makefile-based build system with targets for:

- Building the main executable
- Running unit tests
- Cleaning build artifacts

See [Makefile](file:///D:/SynthFlow/synthflow/Makefile) for details.

## Testing

Unit tests are provided for each major component:

- Lexer tests in [test_lexer.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_lexer.cpp)
- Parser tests in [test_parser.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_parser.cpp)
- Semantic analyzer tests in [test_semantic.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_semantic.cpp)