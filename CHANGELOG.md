# Changelog

All notable changes to the SynthFlow project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.31] - 2025-12-26

### 🚀 WebAssembly Transpilation Support

This release introduces **WebAssembly (Wasm)** as a compilation target, enabling high-performance execution of SynthFlow programs in web browsers and serverless environments.

---

### 🔧 Compiler Enhancements

- **WebAssembly Text (WAT) Transpiler**
  - New `WasmTranspiler` class generating valid WAT S-expression format
  - Full support for numeric operations using `f64` (IEEE 754 double precision)
  - Function declarations with parameters and return types
  - Control flow: `if/else`, `while`, `for` loops mapped to Wasm constructs
  - Local variable management with indexed locals

- **CLI Integration**
  - New transpilation targets: `-t wasm` and `-t wat`
  - Usage: `synthflow transpile program.sf -t wasm -o program.wat`

---

### 📁 New Files

| File | Description |
|------|-------------|
| `compiler/include/wasm_transpiler.h` | WasmTranspiler class declaration |
| `compiler/src/codegen/wasm_transpiler.cpp` | WAT code generation implementation |
| `tests/test_wasm_standalone.cpp` | Standalone verification test harness |

---

### 🏗️ Build System

- Updated `CMakeLists.txt` with `wasm_transpiler` library target
- Added `wasm_transpiler` dependency to main `synthflow` executable

---

### 📖 Technical Details

The WebAssembly transpiler generates WAT (WebAssembly Text) format which can be:
1. Compiled to binary `.wasm` using `wat2wasm` (WebAssembly Binary Toolkit)
2. Loaded directly in browsers using text-format loaders
3. Integrated with JavaScript host environments via import bindings

**Generated Module Structure:**
```wat
(module
  (import "env" "print_f64" (func $print_f64 (param f64)))
  (memory (export "memory") 1)
  (func $add (export "add") (param $a f64) (param $b f64) (result f64)
    (local.get $a) (local.get $b) (f64.add)
    (return)
  )
)
```

---

## [0.0.30] - 2025-12-14

### 🎉 Major Release - Python Parity & Ecosystem Expansion


### 🖥️ Python CLI Parity
- **CLI11 Integration**: Modern command-line argument parsing
- **Inline Code Execution** (`-c`): `synthflow -c "print(42)"`
- **Module Execution** (`-m`): `synthflow -m stdlib/math`
- **Interactive REPL**: `synthflow repl` with persistent environment and multi-line input
- **Check Command**: `synthflow check file.sf` for type-checking only
- **Colored Terminal Output**: ANSI colors for errors, info, and success
- **CLI Options**: `-v` verbose, `-q` quiet, `-O` optimize, `-i` interactive

---

### 🌐 Web Framework
- **Minimal API**: `route("/", json(data))` one-liner syntax
- **HTTP Server**: Native socket-based server (`http_server.cpp`)
- **Response Builders**: `json()`, `html()`, `text()` functions
- **Route Parameters**: `/users/:id` with `req.params.id`
- **Query Parameters**: Access via `req.query`
- **New stdlib module**: `stdlib/web.sf` with helpers

---

### 📱 Mobile Development
- **React Native Target**: `synthflow transpile app.sf --target react-native -o App.js`
- **PWA Target**: `synthflow transpile app.sf --target pwa -o app.js`
- **Mobile Component Library**: `stdlib/mobile.sf` with View, Text, Button, etc.
- **PWA Module**: `stdlib/pwa.sf` with manifest and service worker helpers

---

### 🔌 Model Context Protocol (MCP)
- **Native C++ MCP Server**: `synthflow-mcp` for AI IDE integration
- **Tool Exposure**: Run SynthFlow programs from VS Code, Claude Desktop
- **JSON-RPC over stdio**: Standard MCP communication protocol

---

### 📊 Python Replacement: Data Science & ML

#### Data Science & Visualization
- **`stdlib/plotting.sf`** (550+ lines): ASCII bar, line, scatter, histogram, pie, box, heat maps
- **`dataframe.sf` enhancements** (1,278 lines): Pivot tables, rolling stats, EMA, correlation matrix

#### Machine Learning (TensorFlow/PyTorch Equivalent)
- **`neural_network.sf`** (1,241 lines): Conv2D, MaxPooling2D, LSTM, GRU layers
- **Model serialization**: `saveModel()` and `loadModel()` for persistence

#### Web Scraping (BeautifulSoup Equivalent)
- **`stdlib/scraping.sf`** (735 lines): HTML parsing, CSS selectors, XPath queries
- **Convenience scrapers**: `scrapeLinks()`, `scrapeImages()`, `scrapeTable()`, `scrapeForms()`

#### System Administration
- **`stdlib/sysadmin.sf`** (541 lines): Process management, filesystem ops, network utilities
- **Service control**: `startService`, `stopService`, `restartService`
- **Archiving**: `createTar`, `extractTar`, `createZip`, `extractZip`

#### Enterprise Features
- **`stdlib/database.sf`** (539 lines): Query builder, ORM-like CRUD, transactions, migrations
- **`stdlib/logging.sf`** (373 lines): Log levels, handlers, structured logging
- **`stdlib/testing.sf`** (495 lines): Unit test framework with assertions
- **`stdlib/config.sf`** (501 lines): Configuration loading, validation, merging

---

### 🎨 Branding & Icons
- **SynthFlow Logo**: `assets/logo.png` for documentation and branding
- **VS Code Integration**: Custom SF icon for `.sf` files
- **Installer Branding**: Icon included in Windows installer

---

### 🐛 Bug Fixes
- Fixed `std::get` runtime variant error in Lexer
- Resolved ownership issues in AST construction
- Fixed if/else control flow parsing edge cases
- Added missing `#include <algorithm>` to modules.h

---

### 📈 Summary Statistics
- **40 stdlib modules** (up from 20+)
- **~15,000+ lines** of new/enhanced stdlib code
- **7 new Python-replacement modules**
- **2 major module enhancements** (dataframe, neural_network)
- Cross-platform: Windows, Linux, macOS

---

## [0.0.25] - 2025-12-12

### Added - Quantum Computing
- Native C++ MCP server (`mcp-server/src/main.cpp`)
- JSON utilities for MCP (`mcp-server/src/json_mcp.h`)
- Tool exposure: `run_kenneth` for program execution
- IDE integration support (VS Code, Claude Desktop)

### Added - CI/CD
- GitHub Actions CI workflow (`.github/workflows/ci.yml`)
- Updated release workflow for MCP packaging

### Changed
- Updated `CMakeLists.txt` with `synthflow-mcp` target
- Updated `build.bat` for local MCP development
- Updated documentation (README.md, CHANGELOG.md)

## [0.0.17] - 2025-12-10

### Added - Phase 1: Core Language
- Error reporter with source location tracking and colored output
- Standard I/O library (`stdlib/io.sf`)

### Added - Phase 2: Type System
- Type inference engine (`compiler/include/types.h`)
- Generic types support
- TypeChecker for compile-time validation

### Added - Phase 3: Standard Library
- String manipulation (`stdlib/string.sf`)
- Math functions (`stdlib/math.sf`) - abs, sqrt, sin, cos, pow, gcd, fibonacci
- Collections: List (`stdlib/list.sf`), Map (`stdlib/map.sf`), Set (`stdlib/set.sf`)
- DateTime utilities (`stdlib/datetime.sf`)

### Added - Phase 5: Tooling
- LSP document manager (`lsp-server/src/document_manager.h`)
- Diagnostics, completion, and hover providers
- Enhanced VS Code extension support

### Added - Phase 6: Module System
- Import/export syntax support (`compiler/include/modules.h`)
- Module resolver with dependency graph
- Package manager CLI (`tools/synthpkg/synthpkg.sf`)

### Added - Phase 7: Advanced Features
- Structs and classes (`compiler/include/structs.h`)
- Traits/interfaces with TypeRegistry
- Async/await runtime (`runtime/async_runtime.h`)
- Metaprogramming system (`compiler/include/metaprogramming.h`)

### Added - Gemini API Integration
- AI module (`stdlib/ai.sf`) for LLM integration
- HTTP client (`stdlib/http.sf`, `compiler/include/http_client.h`)
- JSON utilities (`stdlib/json.sf`)
- Agent system (`stdlib/agent.sf`)
- Vector store (`stdlib/vectorstore.sf`)

### Changed
- Updated fibonacci.sf example with correct syntax
- Enhanced example programs for lambda and match expressions

## [0.0.3] - 2024-12-08

### Added
- `const` keyword for immutable variables
- Optional type annotations (`let x: int = 10`)
- Null safety with nullable types (`int?`)
- Try/catch error handling
- -O3 optimized build
- Constant folding optimizer
- Dead code elimination
- Bytecode compiler and VM foundation

## [0.0.2] - 2024-12-07

### Added
- Interpreter for runtime execution
- JavaScript transpiler
- 8 built-in functions (print, input, len, str, int, float, read_file, write_file)
- Increment/decrement operators (++, --)
- Compound assignment operators (+=, -=, *=, /=)

## [0.0.1] - 2024-12-06

### Added
- Initial release
- Lexer with tokenization
- Parser with recursive descent
- AST implementation
- Semantic analyzer
- Code generator
- Control flow (if/else, while, for, break, continue)
- Arrays (literals, indexing, assignment)
- Functions
- VS Code syntax highlighting