# SynthFlow Programming Language

SynthFlow is an AI-native programming language designed to bridge the gap between human intent and machine execution. It features a clean, readable syntax with powerful constructs for modern software development.

## Features

- **Clean Syntax**: Easy to read and write
- **AI-Native**: Designed to work seamlessly with AI systems
- **Intent-Based**: Focuses on what the programmer wants to achieve
- **Control Flow**: Support for if/else, while, for loops, break, and continue statements
- **Arrays**: First-class array support with literals, indexing, and assignment
- **Functions**: First-class function support
- **Extensible**: Designed for rich ecosystem of libraries and tools

## Language Syntax

```synthflow
// Variable declarations
let x = 42;
let y = 3.14;
let name = "Hello, World!";
let is_active = true;

// Functions
fn add(a, b) {
    return a + b;
}

// Control flow
if (result > 50) {
    let message = "Result is greater than 50";
} else {
    let message = "Result is 50 or less";
}

// While loop
let counter = 0;
while (counter < 10) {
    counter = counter + 1;
    if (counter == 3) {
        continue;
    }
    if (counter == 7) {
        break;
    }
    let temp = counter * 2;
}

// For loop
for (let i = 0; i < 5; i = i + 1) {
    if (i == 2) {
        continue;
    }
    if (i == 4) {
        break;
    }
    let square = i * i;
}

// Arrays
let numbers = [1, 2, 3, 4, 5];
let empty_array = [];
let nested_arrays = [[1, 2], [3, 4], [5, 6]];

// Array indexing
let first_number = numbers[0];
let nested_element = nested_arrays[1][0];

// Array assignment
numbers[2] = 10;
nested_arrays[0][1] = 20;
```

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)
- GNU Make
- Windows, macOS, or Linux operating system

### Building

```bash
cd synthflow
make all
```

This will compile:
- The main SynthFlow compiler executable
- All test executables

### Running Tests

```bash
make test
```

## Project Structure

```
synthflow/
├── Makefile              # Build configuration
├── compiler/
│   ├── include/          # Header files
│   ├── src/              # Source files
│   │   ├── lexer/        # Lexer implementation
│   │   ├── parser/       # Parser implementation
│   │   ├── ast/          # AST node implementations
│   │   ├── semantic/     # Semantic analyzer
│   │   ├── codegen/      # Code generator
│   │   └── main.cpp      # Main compiler entry point
│   └── tests/            # Unit tests
├── examples/             # Example SynthFlow programs
└── docs/                 # Documentation
```

## Components

### Lexer (Tokenization)
The lexer converts source code into a stream of tokens, handling:
- Keyword recognition
- Identifier and literal parsing
- Operator and delimiter identification
- Whitespace and comment handling

### Parser (Syntax Analysis)
The parser transforms the token stream into an Abstract Syntax Tree (AST), implementing:
- Recursive descent parsing
- Operator precedence handling
- Grammar rule enforcement

### Semantic Analyzer
The semantic analyzer checks the semantic correctness of the parsed AST:
- Symbol table management
- Declaration checking
- Scope analysis

### Code Generator
The code generator transforms the validated AST into executable code:
- Visitor pattern implementation
- Intermediate representation generation

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.