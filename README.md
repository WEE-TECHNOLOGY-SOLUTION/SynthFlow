# SynthFlow Programming Language

![SynthFlow Logo](assets/logo.png)

[![Build](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions/workflows/ci.yml/badge.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions)
[![Version](https://img.shields.io/badge/version-0.0.3-blue.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

SynthFlow is an AI-native programming language designed to bridge the gap between human intent and machine execution. It features a clean, readable syntax with powerful constructs for modern software development.

## 🚀 Features

- **Clean Syntax**: Easy to read and write
- **AI-Native**: Designed to work seamlessly with AI systems
- **Safety Features**: `const`, type annotations, null safety, try/catch
- **High Performance**: Optimized interpreter with bytecode VM
- **Control Flow**: if/else, while, for loops, break, continue
- **Arrays**: First-class array support with literals and indexing
- **Functions**: First-class function support
- **JavaScript Transpilation**: Compile to JavaScript
- **Extensible**: Rich ecosystem of libraries and tools

## 📦 Quick Start

```bash
# Clone the repository
git clone https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow.git
cd synthflow

# Build (Windows)
.\build.bat

# Build (Linux/Mac)
make all

# Run a program
./synthflow run examples/demo.sf

# Transpile to JavaScript
./synthflow transpile examples/demo.sf
```

## 📖 Language Syntax

### Variables and Constants

```synthflow
// Mutable variables
let x = 42
let name = "Alice"

// Immutable constants
const PI = 3.14159
const MAX_SIZE = 100

// Type annotations (optional)
let count: int = 10
let price: float = 19.99
let active: bool = true

// Nullable types
let maybeValue: int? = null
```

### Functions

```synthflow
fn greet(name) {
    print("Hello,", name)
}

fn add(a, b) {
    return a + b
}

fn fibonacci(n) {
    if (n <= 1) { return n }
    return fibonacci(n - 1) + fibonacci(n - 2)
}
```

### Control Flow

```synthflow
// If/else
if (x > 10) {
    print("Big number")
} else {
    print("Small number")
}

// While loop
let i = 0
while (i < 5) {
    print(i)
    i = i + 1
}

// For loop
for (let j = 0; j < 10; j = j + 1) {
    if (j == 5) { break }
    print(j)
}
```

### Error Handling

```synthflow
try {
    let result = riskyOperation()
    print("Success:", result)
} catch (error) {
    print("Error:", error)
}
```

### Arrays

```synthflow
let numbers = [1, 2, 3, 4, 5]
let first = numbers[0]
numbers[2] = 10

// Nested arrays
let matrix = [[1, 2], [3, 4]]
let element = matrix[1][0]
```

## 🛡️ Safety Features

| Feature | Syntax | Description |
|---------|--------|-------------|
| **const** | `const PI = 3.14` | Immutable variables |
| **Type Annotations** | `let x: int = 10` | Optional type hints |
| **Null Safety** | `let x: int? = null` | Nullable types |
| **Try/Catch** | `try { } catch (e) { }` | Error handling |

## ⚡ Performance

SynthFlow v0.0.3 includes significant performance optimizations:

- **-O3 Optimized Build**: Maximum compiler optimizations
- **Constant Folding**: `1 + 2` computed at compile time
- **Dead Code Elimination**: Removes unreachable code
- **Bytecode Compiler**: 30+ opcodes for fast execution
- **Stack-based VM**: Ready for high-performance execution

**Benchmark:** `fib(25)` completes in ~0.19 seconds

## 🔧 Built-in Functions

| Function | Description | Example |
|----------|-------------|---------|
| `print(...)` | Print to stdout | `print("Hello")` |
| `input(prompt)` | Read user input | `let name = input("Name: ")` |
| `len(x)` | Get length | `len([1, 2, 3])` → 3 |
| `str(x)` | Convert to string | `str(42)` → "42" |
| `int(x)` | Convert to integer | `int("42")` → 42 |
| `float(x)` | Convert to float | `float("3.14")` → 3.14 |
| `read_file(path)` | Read file contents | `read_file("data.txt")` |
| `write_file(path, data)` | Write to file | `write_file("out.txt", "Hello")` |

## 📁 Project Structure

```
synthflow/
├── compiler/
│   ├── include/          # Header files
│   │   ├── lexer.h       # Tokenizer
│   │   ├── parser.h      # Parser
│   │   ├── ast.h         # AST nodes
│   │   ├── interpreter.h # Interpreter
│   │   ├── optimizer.h   # Code optimizer
│   │   └── bytecode.h    # Bytecode VM
│   └── src/              # Implementation
├── examples/             # Example programs
├── docs/                 # Documentation
├── ide-plugins/          # IDE support
│   └── vscode/           # VS Code extension
└── lsp-server/           # Language Server Protocol
```

## 🔨 CLI Commands

```bash
# Run a SynthFlow program
./synthflow run <file.sf>

# Compile and show IR
./synthflow compile <file.sf>

# Transpile to JavaScript
./synthflow transpile <file.sf>

# Show help
./synthflow --help
```

## 📚 Documentation

- [Language Guide](docs/SYNTHFLOW_USAGE_GUIDE.md)
- [API Reference](docs/api-reference.md)
- [Architecture](docs/architecture.md)
- [Contributing](docs/contributing.md)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🏷️ Changelog

### v0.0.3 (Latest)
- ✨ Added `const` keyword for immutable variables
- ✨ Added optional type annotations
- ✨ Added null safety with nullable types
- ✨ Added try/catch error handling
- ⚡ Added -O3 optimized build
- ⚡ Added constant folding optimizer
- ⚡ Added dead code elimination
- ⚡ Added bytecode compiler and VM

### v0.0.2
- ✨ Added interpreter
- ✨ Added JavaScript transpiler
- ✨ Added 8 built-in functions

### v0.0.1
- 🎉 Initial release
- ✨ Basic language constructs