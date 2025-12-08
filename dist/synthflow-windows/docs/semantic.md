# Semantic Analysis Documentation

The semantic analysis phase of the SynthFlow compiler checks the semantic correctness of the program and gathers type information.

## Overview

Semantic analysis is the third phase of the compiler, following lexical analysis and parsing. It verifies that the program is semantically correct and collects type information for code generation.

## Responsibilities

### Type Checking

- Verify that operations are performed on compatible types
- Check function call arguments against parameter types
- Ensure return statements match function return types

### Symbol Resolution

- Resolve identifiers to their declarations
- Build symbol tables for scopes
- Handle name shadowing

### Control Flow Analysis

- Check that all code paths return a value (for non-void functions)
- Verify break/continue statements are within loops
- Validate switch statement cases

### Error Detection

- Undefined variables
- Undefined functions
- Type mismatches
- Duplicate declarations

## Architecture

```
Parser -> AST -> Semantic Analyzer -> Typed AST -> Code Generator
```

## Symbol Table

The symbol table tracks identifiers and their properties:

### Symbol Information

- Name
- Type
- Scope
- Declaration location
- Usage information

### Scope Management

- Global scope
- Function scopes
- Block scopes
- Nested scopes

### Implementation

The symbol table is implemented as a stack of hash maps, one for each scope level.

## Type System

### Primitive Types

- `Int` - 64-bit signed integer
- `Float` - 64-bit floating point
- `Bool` - Boolean value (true/false)
- `String` - String of characters
- `Void` - No value

### Composite Types

- Arrays
- Structs
- Functions
- Optionals (planned)

### Type Inference

SynthFlow supports type inference for variable declarations:

```synthflow
let x = 42        // x is inferred as Int
let y = 3.14      // y is inferred as Float
let z = "hello"   // z is inferred as String
```

## Type Checking Process

### Expression Type Checking

1. Traverse the AST
2. For each expression, determine its type
3. Verify that operations are valid for the types involved

### Statement Type Checking

1. Check variable declarations
2. Verify assignment compatibility
3. Validate control flow statements

### Function Type Checking

1. Check function signatures
2. Verify parameter types
3. Ensure return type compatibility

## Error Reporting

### Error Categories

- Type errors
- Undefined identifiers
- Duplicate declarations
- Control flow errors

### Error Information

- Error message
- Source location (file, line, column)
- Context information
- Suggested fixes (when possible)

### Example Error Messages

```
error: Type mismatch in assignment
  ┌─ test.sf:3:5
  │
3 │ let x: Int = "hello"
  │     ^ expected Int, found String

error: Undefined variable 'y'
  ┌─ test.sf:5:10
  │
5 │ print(y + 1)
  │          ^ undefined variable
```

## Implementation Plan

### Directory Structure

```
compiler/
├── src/
│   ├── semantic/
│   │   ├── analyzer.cpp     # Main semantic analyzer
│   │   ├── typechecker.cpp  # Type checking implementation
│   │   ├── symboltable.cpp  # Symbol table implementation
│   │   └── errors.cpp       # Error reporting
│   └── ...
└── ...
```

## Symbol Resolution

### Name Lookup

1. Search current scope
2. Search parent scopes
3. Report error if not found

### Declaration Checking

- Ensure variables are declared before use
- Prevent duplicate declarations in same scope
- Handle forward references

## Type Inference Algorithm

### Algorithm W ( Hindley-Milner )

SynthFlow uses a variant of the Hindley-Milner type inference algorithm:

1. Assign type variables to expressions
2. Generate constraints based on usage
3. Solve constraints to determine concrete types

### Constraint Generation

- Function application: argument types must match parameter types
- Binary operations: operand types must be compatible
- Return statements: expression type must match function return type

### Constraint Solving

- Unify type variables with concrete types
- Detect type conflicts
- Infer polymorphic types

## Control Flow Analysis

### Return Analysis

- Ensure all code paths in non-void functions return a value
- Detect unreachable code after return statements

### Loop Analysis

- Validate break/continue statements are within loops
- Check loop variable usage

### Conditional Analysis

- Verify all switch cases are handled
- Check for unreachable patterns

## Optimization Opportunities

### Constant Folding

- Evaluate constant expressions at compile time
- Replace expressions with their computed values

### Dead Code Elimination

- Remove code that cannot be executed
- Remove unused variables

### Type Specialization

- Generate optimized code for specific types
- Inline polymorphic functions when types are known

## Integration with Other Phases

### Parser Integration

- Receive AST from parser
- Annotate AST nodes with type information
- Pass typed AST to code generator

### Code Generator Integration

- Provide type information for code generation
- Supply symbol table for name resolution
- Supply optimized AST for code emission

## Testing

### Unit Tests

- Test individual type checking rules
- Test symbol table operations
- Test error detection and reporting

### Integration Tests

- Test complete semantic analysis on example programs
- Verify correct error reporting
- Check type inference results

## Future Enhancements

### Generic Types

Support for parametric polymorphism:

```synthflow
fn identity<T>(x: T) -> T {
    return x
}
```

### Traits/Interfaces

Support for type classes or interfaces:

```synthflow
trait Printable {
    fn print(self)
}

impl Printable for Int {
    fn print(self) {
        // implementation
    }
}
```

### Lifetime Analysis

For memory safety without garbage collection:

```synthflow
fn borrow(x: &Int) -> &Int {
    return x
}
```

## Performance Considerations

### Incremental Analysis

- Cache analysis results
- Re-analyze only changed parts
- Support for IDE features

### Parallel Analysis

- Analyze independent functions in parallel
- Distribute work across multiple cores
- Minimize synchronization overhead

## Debugging Support

### Debug Information

- Track variable lifetimes
- Record type information
- Preserve source locations

### IDE Integration

- Provide real-time feedback
- Support for hover information
- Support for go-to-definition