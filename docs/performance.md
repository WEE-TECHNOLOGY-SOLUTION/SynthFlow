# SynthFlow Performance Guide

SynthFlow v0.0.3 includes significant performance optimizations for faster code execution.

## Build Optimizations

SynthFlow is compiled with maximum optimization flags:

```bash
-O3             # Maximum optimization level
-march=native   # CPU-specific optimizations
-flto           # Link-time optimization
-DNDEBUG        # Disable debug assertions
```

### Building for Performance

```bash
# Windows
.\build.bat

# Linux/Mac
make release
```

---

## Compile-Time Optimizations

### 1. Constant Folding

The compiler pre-computes constant expressions at compile time.

```synthflow
// Before optimization (runtime)
let x = 1 + 2 + 3

// After optimization (compile time)
let x = 6  // Pre-computed!
```

**What gets optimized:**
- Arithmetic: `1 + 2` → `3`
- Boolean: `true && false` → `false`
- Comparisons: `5 > 3` → `true`
- Negation: `-(-5)` → `5`

### 2. Dead Code Elimination

The optimizer removes unreachable code.

```synthflow
// This code:
if (false) {
    print("Never runs")  // Removed!
}

// Becomes empty (optimized away)
```

**What gets removed:**
- `if (false)` blocks
- `while (false)` loops
- Code after `return` statements

---

## Bytecode Compiler

SynthFlow includes a bytecode compiler infrastructure for future VM execution.

### Instruction Set

| OpCode | Description |
|--------|-------------|
| `PUSH_INT` | Push integer to stack |
| `PUSH_FLOAT` | Push float to stack |
| `PUSH_STRING` | Push string to stack |
| `ADD`, `SUB`, `MUL`, `DIV` | Arithmetic |
| `EQ`, `NE`, `LT`, `GT` | Comparisons |
| `JUMP`, `JUMP_IF_FALSE` | Control flow |
| `CALL`, `RETURN` | Functions |
| `LOAD_VAR`, `STORE_VAR` | Variables |

---

## Benchmarks

### Fibonacci Benchmark

```synthflow
fn fib(n) {
    if (n <= 1) { return n }
    return fib(n - 1) + fib(n - 2)
}

print(fib(25))  // Result: 75025
```

| Input | Time |
|-------|------|
| `fib(20)` | ~20ms |
| `fib(25)` | ~190ms |
| `fib(30)` | ~2.1s |

---

## Performance Tips

### 1. Use Constants

```synthflow
// Good: compile-time constant
const MULTIPLIER = 2

// Less optimal: runtime variable
let multiplier = 2
```

### 2. Avoid Redundant Calculations

```synthflow
// Bad: recalculates every iteration
while (i < len(array)) { ... }

// Good: calculate once
let size = len(array)
while (i < size) { ... }
```

### 3. Early Returns

```synthflow
// Good: exit early
fn validate(x) {
    if (x < 0) { return false }
    if (x > 100) { return false }
    return true
}
```

---

## Architecture

```
Source Code (.sf)
       │
       ▼
    Lexer → Tokens
       │
       ▼
    Parser → AST
       │
       ▼
   Optimizer ← Constant Folding
       │        Dead Code Elimination
       ▼
  Interpreter (current)
       │
       ▼
  Bytecode VM (future)
```

---

## Future Optimizations

- [ ] Just-In-Time (JIT) compilation
- [ ] Inline caching for hot paths
- [ ] Loop unrolling
- [ ] Tail call optimization