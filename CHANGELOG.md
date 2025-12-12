# Changelog

All notable changes to the SynthFlow project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.26] - 2025-12-13

### Added - Model Context Protocol (MCP)
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