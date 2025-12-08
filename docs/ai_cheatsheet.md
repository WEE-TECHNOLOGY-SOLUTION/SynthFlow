# SynthFlow Language Cheatsheet

## Basic Syntax

### Variables
```synthflow
let x = 42;
let y: float = 3.14;
let is_active: bool = true;
let message: string = "Hello";
```

### Functions
```synthflow
fn calculate(x: int) -> int {
    return x * 2;
}
```

### Control Flow
```synthflow
if x > 10 {
    print("Greater");
} else {
    print("Smaller");
}

while x > 0 {
    x = x - 1;
}
```

### Data Structures
```synthflow
struct Point {
    x: int;
    y: int;
}

let p = Point { x: 10, y: 20 };
```

### Arrays
```synthflow
let numbers: int[] = [1, 2, 3];
```

## Keywords
- `fn`: Function definition
- `let`: Variable declaration
- `struct`: Structure definition
- `enum`: Enumeration
- `return`: Return value
- `true` / `false`: Booleans
- `if` / `else`: Conditional
- `while` / `for`: Loops
