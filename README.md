# SynthFlow Programming Language

![SynthFlow Logo](assets/logo.png)

[![Build](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions/workflows/ci.yml/badge.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions)
[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

SynthFlow is an **AI-native programming language** designed to bridge the gap between human intent and machine execution. It features a clean, readable syntax with powerful constructs for modern software development and native AI integration.

## ✨ Key Features

### Core Language
- **Clean Syntax** – Easy to read and write
- **Type Safety** – Optional type annotations, null safety
- **Control Flow** – if/else, while, for loops, break, continue
- **Functions** – First-class functions with lambdas
- **Error Handling** – try/catch blocks
- **Collections** – Arrays and maps with literal syntax

### AI & Agent Development (SADK)
- **🤖 Native AI Integration** – Built-in Gemini API support
- **🧠 Agent Framework** – Create intelligent AI agents
- **📡 HTTP Client** – Native HTTP requests
- **📊 Vector Store** – RAG support for AI applications

## 🚀 Quick Start

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
```

## 🤖 AI Integration (Gemini)

SynthFlow has **native Google Gemini API integration**:

```synthflow
// Set your API key
gemini_set_api_key("YOUR_API_KEY")

// Simple completion
let response = gemini_complete("What is 2+2?", "gemini-2.0-flash")
print(response)

// Chat with system prompt
let answer = gemini_chat(
    "You are a helpful assistant.",
    "Explain quantum computing briefly.",
    "gemini-2.0-flash"
)
print(answer)
```

📖 **[Full Gemini Integration Guide](docs/gemini-integration.md)**

### AI Built-in Functions

| Function | Description |
|----------|-------------|
| `gemini_set_api_key(key)` | Configure API key |
| `gemini_has_api_key()` | Check if key is set |
| `gemini_complete(prompt, model)` | Text completion |
| `gemini_chat(system, user, model)` | Chat completion |

## 📖 Language Syntax

### Variables and Constants

```synthflow
// Mutable variables
let x = 42
let name = "Alice"

// Immutable constants
const PI = 3.14159

// Type annotations (optional)
let count: int = 10
let active: bool = true

// Nullable types
let maybeValue: int? = null
```

### Maps (New!)

```synthflow
// Map literals
let person = {
    name: "Alice",
    age: 30,
    city: "Tokyo"
}

// Member access
print(person.name)
print(person.age)
```

### Lambda Functions (New!)

```synthflow
// Arrow function syntax
let double = (x) => x * 2
let add = (a, b) => a + b

print(double(5))   // 10
print(add(3, 4))   // 7
```

### String Interpolation (New!)

```synthflow
let name = "World"
let greeting = "Hello, ${name}!"
print(greeting)  // Hello, World!
```

### Functions

```synthflow
fn greet(name) {
    print("Hello, " + name)
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
    print("Big")
} else {
    print("Small")
}

// For loop
for (let i = 0; i < 5; i = i + 1) {
    print(i)
}

// While loop
while (condition) {
    // ...
}
```

### Error Handling

```synthflow
try {
    let result = riskyOperation()
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
```

## 📦 Standard Library (SADK)

| Module | Description | Status |
|--------|-------------|--------|
| `stdlib/ai.sf` | AI model integration | ✅ Ready |
| `stdlib/agent.sf` | Agent framework | ✅ Ready |
| `stdlib/http.sf` | HTTP client | ✅ Ready |
| `stdlib/json.sf` | JSON utilities | ✅ Ready |
| `stdlib/vectorstore.sf` | Vector storage/RAG | ✅ Ready |
| `stdlib/io.sf` | File I/O operations | ✅ Ready |

📖 **[SADK Documentation](docs/agent-development-kit.md)**

## 🔧 Built-in Functions

### Core Functions

| Function | Description |
|----------|-------------|
| `print(...)` | Print to stdout |
| `input(prompt)` | Read user input |
| `len(x)` | Get length |
| `str(x)` | Convert to string |
| `int(x)` | Convert to integer |
| `float(x)` | Convert to float |

### File I/O

| Function | Description |
|----------|-------------|
| `read_file(path)` | Read file contents |
| `write_file(path, data)` | Write to file |

### HTTP (New!)

| Function | Description |
|----------|-------------|
| `http_get(url)` | HTTP GET request |
| `http_post(url, body)` | HTTP POST request |

## 📁 Project Structure

```
synthflow/
├── compiler/
│   ├── include/          # Headers
│   │   ├── lexer.h
│   │   ├── parser.h
│   │   ├── ast.h
│   │   ├── interpreter.h
│   │   └── http_client.h # NEW: HTTP/Gemini
│   └── src/
│       └── http/         # NEW: HTTP client
├── stdlib/               # NEW: Standard library
│   ├── ai.sf
│   ├── agent.sf
│   ├── http.sf
│   ├── json.sf
│   └── vectorstore.sf
├── examples/
├── docs/
│   ├── gemini-integration.md  # NEW
│   └── agent-development-kit.md
└── ide-plugins/
    └── vscode/
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
- [Gemini Integration](docs/gemini-integration.md)
- [Agent Development Kit](docs/agent-development-kit.md)
- [API Reference](docs/api-reference.md)
- [Architecture](docs/architecture.md)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🏷️ Changelog

### v0.0.17 (Latest)
- 🤖 **Native Gemini API integration**
- 🧠 **SADK (SynthFlow Agent Development Kit)**
  - AI module with completion/chat
  - Agent framework
  - HTTP client
  - JSON utilities
  - Vector store for RAG
- ✨ Map literals `{ key: value }`
- ✨ Member access `obj.field`
- ✨ Lambda functions `(x) => x * 2`
- ✨ String interpolation `"Hello, ${name}!"`
- ✨ Struct declarations
- 🐛 Fixed array literal parsing bug

### v0.0.3
- ✨ Added `const` keyword for immutable variables
- ✨ Added optional type annotations
- ✨ Added null safety with nullable types
- ✨ Added try/catch error handling
- ⚡ Added -O3 optimized build
- ⚡ Added constant folding optimizer

### v0.0.2
- ✨ Added interpreter
- ✨ Added JavaScript transpiler
- ✨ Added 8 built-in functions

### v0.0.1
- 🎉 Initial release
- ✨ Basic language constructs