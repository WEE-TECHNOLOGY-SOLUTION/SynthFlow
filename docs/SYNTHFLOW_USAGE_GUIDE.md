# SynthFlow Language Usage Guide

## Overview

SynthFlow is an AI-native programming language designed with a clean, readable syntax and powerful constructs for modern software development.

## Language Features

- **Clean Syntax**: Easy to read and write
- **AI-Native**: Designed to work seamlessly with AI systems
- **Intent-Based**: Focuses on what the programmer wants to achieve
- **Control Flow**: Support for if/else, while, for loops, break, and continue statements
- **Arrays**: First-class array support with literals, indexing, and assignment
- **Functions**: First-class function support

## Basic Syntax

### Variables
```synthflow
let x = 42;
let y = 3.14;
let name = "Hello, World!";
let is_active = true;
```

### Functions
```synthflow
fn add(a, b) {
    return a + b;
}
```

### Control Flow

#### If/Else Statements
```synthflow
if (result > 50) {
    let message = "Result is greater than 50";
} else {
    let message = "Result is 50 or less";
}
```

#### While Loops
```synthflow
let counter = 0;
while (counter < 10) {
    counter = counter + 1;
    if (counter == 3) {
        continue; // Skip the rest of the loop body
    }
    if (counter == 7) {
        break; // Exit the loop
    }
    let temp = counter * 2;
}
```

#### For Loops
```synthflow
for (let i = 0; i < 5; i = i + 1) {
    if (i == 2) {
        continue; // Skip the rest of the loop body
    }
    if (i == 4) {
        break; // Exit the loop
    }
    let square = i * i;
}
```

### Arrays
```synthflow
// Array literals
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

## Compiling and Running Programs

### Basic Compilation
```bash
synthflow.exe program.synth
```

### Running Examples
```bash
synthflow.exe examples/hello.synth
```

## Project Structure

```
synthflow/
├── compiler/           # Compiler source code
│   ├── include/        # Header files
│   ├── src/            # Source files
│   └── tests/          # Unit tests
├── examples/           # Example programs
├── docs/               # Documentation
└── scripts/            # Build and utility scripts
```

## Components

### Lexer (Tokenization)
Converts source code into a stream of tokens, handling:
- Keyword recognition
- Identifier and literal parsing
- Operator and delimiter identification
- Whitespace and comment handling

### Parser (Syntax Analysis)
Transforms the token stream into an Abstract Syntax Tree (AST):
- Recursive descent parsing
- Operator precedence handling
- Grammar rule enforcement

### Semantic Analyzer
Checks the semantic correctness of the parsed AST:
- Symbol table management
- Declaration checking
- Scope analysis

### Code Generator
Transforms the validated AST into executable code:
- Visitor pattern implementation
- Intermediate representation generation

## Creating Your First Program

1. Create a new file with `.synth` extension:
```synthflow
// hello.synth
let message = "Hello, SynthFlow!";
print(message);
```

2. Compile and run:
```bash
synthflow.exe hello.synth
```

## Testing

Run the provided test suite:
```bash
make test
```

Or run individual test executables:
```bash
test_lexer.exe
test_parser.exe
test_semantic.exe
test_codegen.exe
```