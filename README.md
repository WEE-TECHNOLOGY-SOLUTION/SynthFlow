# SynthFlow Programming Language

![SynthFlow Logo](assets/logo.png)

[![Build](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions/workflows/ci.yml/badge.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/actions)
[![Version](https://img.shields.io/badge/version-0.0.31-blue.svg)](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

SynthFlow is an **AI-native programming language** designed to bridge the gap between human intent and machine execution. It features a clean, readable syntax with powerful constructs for modern software development, native AI integration, and now **Quantum Computing** simulation.

## ✨ Key Features

### Core Language
- **Clean Syntax** – Easy to read and write like Python/TypeScript
- **Type Safety** – Optional type annotations, null safety
- **Control Flow** – if/else, while, for loops, break, continue
- **Functions** – First-class functions with lambdas
- **Collections** – Arrays and maps with literal syntax

### ⚛️ Quantum Computing (New!)
- **State Vector Simulation** – Simulate quantum states and amplitudes
- **Complex Numbers** – First-class support for complex arithmetic
- **Quantum Gates** – Hadamard, Pauli-X/Y/Z, CNOT, and more
- **Algorithms** – Helpers for Grover's search, Bell states, etc.

### AI & Agent Development (SADK)
- **🤖 Native AI Integration** – Built-in Gemini API support
- **🧠 Agent Framework** – Create intelligent AI agents
- **📡 HTTP Client** – Native HTTP requests
- **📊 Vector Store** – RAG support for AI applications

### 🔌 Model Context Protocol (MCP)
- **AI IDE Integration** – Connect SynthFlow to VS Code, Claude Desktop, and other AI tools
- **Native C++ Server** – High-performance MCP server (`synthflow-mcp`)
- **Tool Exposure** – Expose SynthFlow capabilities as AI-callable tools

### 🌐 WebAssembly Transpilation (New!)
- **WAT Code Generation** – Transpile SynthFlow to WebAssembly Text format
- **Browser Execution** – Run SynthFlow programs in any modern web browser
- **Serverless Ready** – Deploy to edge computing and serverless platforms
- **CLI Support** – `synthflow transpile app.sf -t wasm -o app.wat`

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow.git
cd synthflow

# Build (Windows)
.\build.bat

# Run a quantum demo
.\synthflow run examples/quantum_demo.sf
```

## ⚛️ Quantum Computing

SynthFlow now supports quantum programming primitives:

```synthflow
import quantum

// Create a 2-qubit system initialized to |00⟩
let qs = quantum.createSystem(2)

// Apply Hadamard gate to first qubit -> Superposition
qs.applyUnknownGate("H", 0)

// Apply CNOT gate (Control: 0, Target: 1) -> Entanglement
qs.applyCNOT(0, 1)

// Measure the system
let result = qs.measureAll()
print("Measurement result: " + result)
```

## 🤖 AI Integration (Gemini)

SynthFlow has **native Google Gemini API integration**:

```synthflow
// Set your API key
gemini_set_api_key("YOUR_API_KEY")

// Chat with system prompt
let answer = gemini_chat(
    "You are a helpful assistant.",
    "Explain quantum computing briefly.",
    "gemini-2.0-flash"
)
print(answer)
```

## 📦 Standard Library

| Module | Description | Status |
|--------|-------------|--------|
| `stdlib/quantum.sf` | Quantum computing simulation | ✅ Ready |
| `stdlib/complex.sf` | Complex number arithmetic | ✅ Ready |
| `stdlib/ai.sf` | AI model integration | ✅ Ready |
| `stdlib/agent.sf` | Agent framework | ✅ Ready |
| `stdlib/http.sf` | HTTP client | ✅ Ready |
| `stdlib/json.sf` | JSON utilities | ✅ Ready |
| `stdlib/io.sf` | File I/O operations | ✅ Ready |
| `stdlib/dataframe.sf` | Pandas-like tabular data (pivot tables, time series) | ✅ Ready |
| `stdlib/neural_network.sf` | Deep learning (CNN, LSTM, GRU) | ✅ Ready |
| `stdlib/classification.sf` | ML classifiers (SVM, KNN, RandomForest) | ✅ Ready |
| `stdlib/regression.sf` | Regression models | ✅ Ready |
| `stdlib/plotting.sf` | ASCII data visualization | ✅ Ready |
| `stdlib/scraping.sf` | Web scraping & HTML parsing | ✅ Ready |
| `stdlib/sysadmin.sf` | System administration | ✅ Ready |
| `stdlib/database.sf` | Database connectivity & ORM | ✅ Ready |
| `stdlib/logging.sf` | Enterprise logging | ✅ Ready |
| `stdlib/testing.sf` | Unit testing framework | ✅ Ready |
| `stdlib/config.sf` | Configuration management | ✅ Ready |
| `stdlib/os.sf` | **NEW** OS utilities, env vars, filesystem | ✅ Ready |
| `stdlib/subprocess.sf` | **NEW** Command execution, pipes, process mgmt | ✅ Ready |
| `stdlib/async.sf` | **NEW** Promises, async utilities, event loop | ✅ Ready |
| `stdlib/regex.sf` | **NEW** Pattern matching, text validation | ✅ Ready |
| `stdlib/crypto.sf` | **NEW** Hashing (SHA, MD5), encryption | ✅ Ready |
| `stdlib/threading.sf` | **NEW** Threads, mutexes, channels, parallel | ✅ Ready |
| `stdlib/networking.sf` | **NEW** TCP/UDP sockets, DNS, port scanning | ✅ Ready |
| `stdlib/security.sf` | **NEW** JWT, OAuth2, input validation, CSRF, CSP | ✅ Ready |
| `stdlib/api.sf` | **NEW** OpenAPI, GraphQL, API Gateway, versioning | ✅ Ready |
| `stdlib/numpy.sf` | **NEW** NumPy-like arrays, linear algebra, vectorized ops | ✅ Ready |
| `stdlib/statistics.sf` | **NEW** Statistical tests, distributions, regression | ✅ Ready |

## 🔧 Built-in Functions

### Math & Numbers
| Function | Description |
|----------|-------------|
| `sqrt(x)` | Square root |
| `pow(x, y)` | Power (x^y) |
| `sin(x)`, `cos(x)` | Trigonometry |
| `abs(x)` | Absolute value |
| `round(x)` | Round to nearest integer |

### Core & Collections
| Function | Description |
|----------|-------------|
| `print(...)` | Print to stdout |
| `len(x)` | Get length of array/string |
| `append(arr, item)` | Add item to array |
| `push(arr, item)` | Alias for append |
| `typeof(x)` | Get type name string |

## 🏷️ Changelog

### v0.0.31 (WebAssembly Update)
- 🌐 **WebAssembly Transpilation Support**
  - New `WasmTranspiler` generating WAT (WebAssembly Text) format
  - CLI integration: `-t wasm` and `-t wat` targets
  - Full numeric operations with `f64` precision
  - Control flow mapping to Wasm constructs

### v0.0.30 (Python Parity)
- 🖥️ **CLI11 Integration**: Modern command-line parsing
- 🌐 **Web Framework**: Native HTTP server with route parameters
- 📱 **Mobile Targets**: React Native and PWA transpilation
- 📊 **Data Science**: DataFrame, Neural Networks, ML classifiers

### v0.0.26 (MCP Update)
- 🔌 **Model Context Protocol (MCP) Server**
  - Native C++ implementation (`synthflow-mcp`)
  - JSON-RPC over stdio communication
  - Tool integration for AI IDEs (VS Code, Claude Desktop)

### v0.0.25 (Quantum Update)
- ⚛️ **Native Quantum Computing Support**
  - Complex number arithmetic and utilities
  - Qubit state simulation and gate operations
  - Quantum standard library (`stdlib/quantum.sf`)