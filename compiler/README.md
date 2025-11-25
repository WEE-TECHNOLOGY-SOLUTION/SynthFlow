# SynthFlow Compiler

This directory contains the implementation of the SynthFlow compiler, which transforms SynthFlow source code into executable code.

## Components

### Lexer
The lexer ([lexer.h](file:///D:/SynthFlow/synthflow/compiler/include/lexer.h), [lexer.cpp](file:///D:/SynthFlow/synthflow/compiler/src/lexer/lexer.cpp)) is responsible for tokenizing the source code into a stream of tokens.

### Parser
The parser ([parser.h](file:///D:/SynthFlow/synthflow/compiler/include/parser.h), [parser.cpp](file:///D:/SynthFlow/synthflow/compiler/src/parser/parser.cpp)) transforms the token stream into an Abstract Syntax Tree (AST).

### AST
The AST ([ast.h](file:///D:/SynthFlow/synthflow/compiler/include/ast.h), [ast_visitor.cpp](file:///D:/SynthFlow/synthflow/compiler/src/ast/ast_visitor.cpp)) defines the node structure for representing the program structure.

### Semantic Analyzer
The semantic analyzer ([semantic_analyzer.h](file:///D:/SynthFlow/synthflow/compiler/include/semantic_analyzer.h), [semantic_analyzer.cpp](file:///D:/SynthFlow/synthflow/compiler/src/semantic/semantic_analyzer.cpp)) checks the semantic correctness of the AST.

### Code Generator
The code generator ([code_generator.h](file:///D:/SynthFlow/synthflow/compiler/include/code_generator.h), [code_generator.cpp](file:///D:/SynthFlow/synthflow/compiler/src/codegen/code_generator.cpp)) transforms the AST into executable code.

## Directory Structure

```
compiler/
├── include/           # Header files
├── src/               # Source files
│   ├── lexer/         # Lexer implementation
│   ├── parser/        # Parser implementation
│   ├── ast/           # AST visitor implementations
│   ├── semantic/      # Semantic analyzer implementation
│   ├── codegen/       # Code generator implementation
│   └── main.cpp       # Main executable source
└── tests/             # Unit tests
```

## Building

To build the compiler, run `make all` from the parent directory.

## Testing

Unit tests are available for each component:
- Lexer tests: [test_lexer.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_lexer.cpp)
- Parser tests: [test_parser.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_parser.cpp)
- Semantic analyzer tests: [test_semantic.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_semantic.cpp)
- Code generator tests: [test_codegen.cpp](file:///D:/SynthFlow/synthflow/compiler/tests/test_codegen.cpp)