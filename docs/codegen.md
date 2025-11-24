# Code Generation Documentation

The code generation phase of the SynthFlow compiler translates the Abstract Syntax Tree (AST) into executable code.

## Overview

The code generator takes the AST produced by the parser and semantic analyzer and generates LLVM Intermediate Representation (IR) code.

## Architecture

```
AST -> Code Generator -> LLVM IR -> Machine Code
```

## LLVM Integration

SynthFlow uses LLVM for code generation, optimization, and machine code generation.

### Benefits of LLVM

- Mature optimization passes
- Support for multiple architectures
- Just-In-Time compilation capabilities
- Extensive documentation and community

### LLVM Version

SynthFlow targets LLVM 17+ for the latest features and improvements.

## Code Generation Process

### 1. LLVM Context Setup

- Create LLVM context
- Create LLVM module
- Set up target triple and data layout

### 2. Type Translation

- Translate SynthFlow types to LLVM types
- Handle primitive types (Int, Float, Bool, String)
- Handle composite types (Arrays, Structs)

### 3. Function Generation

- Create LLVM function for each SynthFlow function
- Generate function parameters
- Create basic blocks for control flow

### 4. Statement Translation

- Translate statements to LLVM IR instructions
- Handle variable declarations
- Handle control flow statements (if, while)
- Handle return statements

### 5. Expression Translation

- Translate expressions to LLVM IR instructions
- Handle literals
- Handle binary operations
- Handle function calls

### 6. Optimization

- Run LLVM optimization passes
- Apply target-specific optimizations

### 7. Code Emission

- Emit object code
- Emit assembly code (optional)
- Emit LLVM bitcode (optional)

## Implementation Plan

### Directory Structure

```
compiler/
├── src/
│   ├── codegen/
│   │   ├── codegen.cpp      # Main code generator
│   │   ├── types.cpp        # Type translation
│   │   ├── functions.cpp    # Function generation
│   │   ├── statements.cpp   # Statement translation
│   │   └── expressions.cpp  # Expression translation
│   └── ...
└── ...
```

## Type System Translation

### Primitive Types

| SynthFlow Type | LLVM Type     |
|----------------|---------------|
| Int            | i64           |
| Float          | double        |
| Bool           | i1            |
| String         | i8* (pointer) |
| Void           | void          |

### Composite Types

- Arrays: LLVM array types
- Structs: LLVM struct types
- Functions: LLVM function types

## Function Generation

### Function Declaration

```synthflow
fn add(x: Int, y: Int) -> Int {
    return x + y
}
```

Translates to LLVM IR:
```llvm
define i64 @add(i64 %x, i64 %y) {
entry:
    %add = add i64 %x, %y
    ret i64 %add
}
```

### Function Calls

Function calls are translated to LLVM `call` instructions.

## Control Flow Translation

### If Statements

If statements are translated to LLVM conditional branches:

```llvm
%cond = icmp ne i1 %condition, 0
br i1 %cond, label %then, label %else
```

### While Loops

While loops are translated to LLVM loops with conditional branches.

## Memory Management

### Stack Allocation

Local variables are allocated on the stack using `alloca`:

```llvm
%var = alloca i64
store i64 42, i64* %var
```

### Heap Allocation

For dynamic allocation, the runtime library is called.

## Error Handling

### Compilation Errors

- Type mismatches
- Undefined variables
- Undefined functions
- Invalid operations

### Runtime Errors

- Division by zero
- Array bounds checking
- Null pointer dereference

## Optimization

### LLVM Optimization Passes

- Instruction combining
- Dead code elimination
- Constant folding
- Loop optimization
- Function inlining

### SynthFlow-Specific Optimizations

- Tail call optimization
- Closure optimization
- Pattern matching optimization

## Target Architecture Support

### Currently Supported

- x86-64
- ARM64
- RISC-V (planned)

### Cross-Compilation

LLVM's cross-compilation support allows building for different targets.

## Debug Information

### DWARF Debug Information

LLVM can generate DWARF debug information for debugging.

### Source Location Tracking

- Track source file and line numbers
- Map AST nodes to IR instructions
- Preserve variable names in debug info

## Just-In-Time Compilation

### LLVM JIT Engine

SynthFlow can use LLVM's JIT engine for immediate execution.

### Use Cases

- REPL (Read-Eval-Print Loop)
- Script execution
- Interactive development

## Future Enhancements

### Profile-Guided Optimization

Using runtime profiling data to guide optimizations.

### Link Time Optimization

Whole-program optimization during linking.

### Machine Learning-Based Optimizations

Using ML models to guide optimization decisions.

## Integration with Runtime

The generated code interfaces with the runtime library for:

- Memory management
- I/O operations
- System calls
- Exception handling

## Testing

### Code Generation Tests

- Test individual IR generation
- Test optimization passes
- Test target code generation

### Integration Tests

- Compile and run example programs
- Verify output correctness
- Performance benchmarking

## Build System Integration

### Makefile Targets

```makefile
# Generate LLVM IR
codegen: $(SRC_FILES)
    $(COMPILER) --emit-llvm $< -o $@

# Generate object files
object: $(SRC_FILES)
    $(COMPILER) --emit-obj $< -o $@
```

### CMake Integration

For projects using CMake, provide a SynthFlow package.

## Command Line Interface

### Output Options

```bash
synthflow --emit-llvm file.sf     # Emit LLVM IR
synthflow --emit-asm file.sf      # Emit assembly
synthflow --emit-obj file.sf      # Emit object file
synthflow --emit-bc file.sf       # Emit bitcode
```

### Optimization Levels

```bash
synthflow -O0 file.sf             # No optimization
synthflow -O1 file.sf             # Basic optimization
synthflow -O2 file.sf             # Standard optimization
synthflow -O3 file.sf             # Aggressive optimization
```