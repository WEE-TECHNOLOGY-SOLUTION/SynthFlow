# Build System

The SynthFlow project uses a Makefile-based build system for compiling the compiler and running tests.

## Prerequisites

- GNU Make
- C++17 compatible compiler (GCC, Clang, MSVC)
- Windows, macOS, or Linux operating system

## Building the Project

To build the entire project, run:

```bash
make all
```

This will compile:
- The main SynthFlow compiler executable
- All test executables

## Build Targets

### Main Executable
- `synthflow.exe` - The main compiler executable

### Test Executables
- `test_lexer.exe` - Lexer unit tests
- `test_parser.exe` - Parser unit tests
- `test_semantic.exe` - Semantic analyzer unit tests
- `test_codegen.exe` - Code generator unit tests

### Utility Targets
- `make clean` - Remove all build artifacts
- `make test` - Build and run all tests

## Directory Structure

```
synthflow/
├── Makefile              # Build configuration
├── compiler/
│   ├── include/          # Header files
│   ├── src/              # Source files
│   │   ├── lexer/        # Lexer implementation
│   │   ├── parser/       # Parser implementation
│   │   ├── ast/          # AST visitor implementations
│   │   ├── semantic/     # Semantic analyzer implementation
│   │   ├── codegen/      # Code generator implementation
│   │   └── main.cpp      # Main executable source
│   └── tests/            # Unit tests
└── scripts/              # Helper scripts
    ├── build.bat         # Windows build script
    ├── test.bat          # Windows test script
    └── run-example.bat   # Example compiler run script
```

## Windows Scripts

For Windows users, helper scripts are provided:

- `scripts/build.bat` - Build the project
- `scripts/test.bat` - Run all tests
- `scripts/run-example.bat` - Run the compiler on the example file

## Makefile Variables

The Makefile uses the following configurable variables:

- `CXX` - C++ compiler (defaults to g++)
- `CXXFLAGS` - Compiler flags (defaults to -std=c++17 -Wall -Wextra -O2)
- `INCLUDES` - Include directories (defaults to -Icompiler/include)
- `LIBS` - Link libraries (defaults to empty)

## Object Files

The build process creates the following object files:
- `lexer.o` - Lexer implementation
- `parser.o` - Parser implementation
- `ast_visitor.o` - AST visitor implementations
- `semantic_analyzer.o` - Semantic analyzer implementation
- `code_generator.o` - Code generator implementation
- `main.o` - Main executable source
- `test_*.o` - Test object files

## Troubleshooting

### Common Issues

1. **"make: command not found"** - Make sure GNU Make is installed and in your PATH
2. **Compiler errors** - Ensure you have a C++17 compatible compiler installed
3. **Linker errors** - Check that all dependencies are correctly specified

### Windows Users

On Windows, you may need to use MinGW or MSYS2 to get GNU Make. Alternatively, you can use the provided batch scripts in the [scripts](file:///D:/SynthFlow/synthflow/scripts) directory.

### Cleaning the Build

If you encounter build issues, try cleaning the build directory:

```bash
make clean
make all
```

This removes all object files and executables, then rebuilds everything from scratch.