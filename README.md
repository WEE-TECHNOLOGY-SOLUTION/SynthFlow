# SynthFlow Programming Language

SynthFlow is an AI-native programming language designed for LLM optimization and unified full-stack development.

## Features

- AI-optimized syntax for better LLM understanding
- Unified full-stack development approach
- Built-in support for modern programming paradigms
- IDE integration with Language Server Protocol support

## Project Structure

```
synthflow/
├── compiler/           # Core compiler implementation
│   ├── src/            # Source files
│   │   ├── lexer/      # Tokenization
│   │   ├── parser/     # Syntax analysis
│   │   ├── semantic/   # Type checking, semantic analysis
│   │   ├── codegen/    # LLVM IR generation
│   │   └── main.cpp    # Compiler entry point
│   ├── include/        # Header files
│   └── tests/          # Compiler tests
├── runtime/            # Runtime library
├── stdlib/             # Standard library
├── lsp-server/         # Language Server Protocol implementation
├── ide-plugins/        # IDE integrations
├── examples/           # Example programs
└── docs/               # Documentation
```

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- LLVM 17+ (for code generation phase)

### Building on Linux/macOS

```bash
./scripts/build.sh
```

### Building on Windows

```cmd
scripts\build.bat
```

## Current Implementation Status

- [x] Project structure setup
- [x] Lexer (Tokenization)
- [ ] Parser (AST Generation)
- [ ] Semantic Analysis
- [ ] Code Generation
- [ ] Runtime Library
- [ ] Standard Library
- [ ] Language Server Protocol
- [ ] IDE Plugins

## Example

```synthflow
# This is a simple SynthFlow program
fn fibonacci(n: Int) -> Int {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

fn main() -> Int {
    let result = fibonacci(10)
    print("Fibonacci of 10 is: " + result)
    return 0
}
```