# How to Install and Build SynthFlow on Windows

## Prerequisites

To build and run SynthFlow on Windows, you need:

1. A C++ compiler that supports C++17
2. GNU Make (for using the existing Makefile)
3. Git Bash or WSL (optional but recommended)

## Option 1: Install MinGW-w64 (Recommended)

1. Download MinGW-w64 from: https://www.mingw-w64.org/downloads/
   - Recommended: Use the winlibs distribution which includes everything needed
   - Download link: https://github.com/brechtsanders/winlibs_mingw/releases

2. Extract to `C:\mingw64`

3. Add `C:\mingw64\bin` to your system PATH:
   - Press Win + X and select "System"
   - Click "Advanced system settings"
   - Click "Environment Variables"
   - Under "System Variables", find and select "Path", then click "Edit"
   - Click "New" and add `C:\mingw64\bin`
   - Click OK to save

4. Restart your command prompt/PowerShell

5. Verify installation:
   ```
   g++ --version
   make --version
   ```

## Option 2: Install Visual Studio Community (Alternative)

1. Download Visual Studio Community from: https://visualstudio.microsoft.com/vs/community/

2. During installation, make sure to select:
   - "Desktop development with C++" workload
   - "C++ CMake tools for Windows" (if using CMake)

## Building SynthFlow

### Method 1: Using MinGW-w64 with Make (Recommended)

After installing MinGW-w64:

```
cd d:\SynthFlow\synthflow
make clean
make all
```

### Method 2: Manual Compilation with g++

If make doesn't work, you can compile manually:

```
cd d:\SynthFlow\synthflow
g++ -std=c++17 -Icompiler/include -c compiler/src/lexer/lexer.cpp -o lexer.o
g++ -std=c++17 -Icompiler/include -c compiler/src/parser/parser.cpp -o parser.o
g++ -std=c++17 -Icompiler/include -c compiler/src/ast/ast_visitor.cpp -o ast_visitor.o
g++ -std=c++17 -Icompiler/include -c compiler/src/semantic/semantic_analyzer.cpp -o semantic_analyzer.o
g++ -std=c++17 -Icompiler/include -c compiler/src/codegen/code_generator.cpp -o code_generator.o
g++ -std=c++17 -Icompiler/include -c compiler/src/main.cpp -o main.o
g++ lexer.o parser.o ast_visitor.o semantic_analyzer.o code_generator.o main.o -o synthflow.exe
```

### Method 3: Using Visual Studio Compiler

If you have Visual Studio installed:

```
cd d:\SynthFlow\synthflow
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cl /std:c++17 /Icompiler/include compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp compiler/src/codegen/code_generator.cpp compiler/src/main.cpp /link /out:synthflow.exe
```

## Running SynthFlow

After successful compilation:

```
synthflow.exe examples/hello.synth
```

## Troubleshooting

1. **"'make' is not recognized"**: Install MinGW-w64 as described above
2. **"'g++' is not recognized"**: Make sure MinGW-w64 bin directory is in your PATH
3. **Compiler errors**: Ensure you're using a C++17 compatible compiler
4. **Missing header files**: Make sure your compiler installation is complete

## Testing

Run the provided test suite:

```
make test
```

Or run individual test executables:
```
test_lexer.exe
test_parser.exe
test_semantic.exe
test_codegen.exe
```