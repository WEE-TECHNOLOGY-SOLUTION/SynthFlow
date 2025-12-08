# SynthFlow Standard Library

The SynthFlow standard library provides essential functions and types for SynthFlow programs.

## Core Modules

### Core Types
- `Int` - Integer type
- `Float` - Floating-point type
- `Bool` - Boolean type
- `String` - String type
- `Void` - Void type

### Collections
- `Array` - Dynamic arrays
- `List` - Linked lists
- `Map` - Hash maps
- `Set` - Hash sets

### I/O Functions
- `print()` - Print to stdout
- `println()` - Print to stdout with newline
- `input()` - Read from stdin

### Math Functions
- `abs()` - Absolute value
- `min()` - Minimum of two values
- `max()` - Maximum of two values
- `sqrt()` - Square root
- `pow()` - Power function

### String Functions
- `len()` - Length of string
- `substr()` - Substring extraction
- `split()` - Split string into array
- `join()` - Join array into string

## Module Organization

```
stdlib/
├── core/        # Core types and functions
├── collections/ # Data structures
├── io/          # Input/output functions
├── math/        # Mathematical functions
├── string/      # String manipulation
├── net/         # Networking functions
└── ai/          # AI-specific functions
```

## Core Module

The core module provides fundamental types and functions.

### Types
```synthflow
type Int      # Integer type
type Float    # Floating-point type
type Bool     # Boolean type (true/false)
type String   # String type
type Void     # Void type
```

### Functions
```synthflow
fn len(collection) -> Int          # Get length of collection
fn str(value) -> String            # Convert value to string
fn int(value) -> Int               # Convert value to integer
fn float(value) -> Float           # Convert value to float
```

## Collections Module

The collections module provides data structures for organizing data.

### Array
```synthflow
let arr = [1, 2, 3, 4, 5]         # Create array
arr[0] = 10                        # Access element
arr.push(6)                        # Add element
arr.pop()                          # Remove last element
```

### Map
```synthflow
let map = {"key": "value"}         # Create map
map["key"] = "new_value"           # Update value
let value = map["key"]             # Access value
```

## I/O Module

The I/O module provides input and output functions.

### Functions
```synthflow
print("Hello, World!")             # Print without newline
println("Hello, World!")           # Print with newline
let input = read()                 # Read from stdin
```

## Math Module

The math module provides mathematical functions.

### Constants
```synthflow
math.PI                            # Pi constant
math.E                             # Euler's number
```

### Functions
```synthflow
math.abs(-5)                       # Absolute value: 5
math.min(3, 7)                     # Minimum: 3
math.max(3, 7)                     # Maximum: 7
math.sqrt(16)                      # Square root: 4
math.pow(2, 3)                     # Power: 8
```

## String Module

The string module provides string manipulation functions.

### Functions
```synthflow
string.len("hello")                # Length: 5
string.substr("hello", 1, 3)       # Substring: "ell"
string.split("a,b,c", ",")         # Split: ["a", "b", "c"]
string.join(["a", "b", "c"], ",")  # Join: "a,b,c"
```

## Net Module (Planned)

The net module will provide networking functions.

### Functions (Planned)
```synthflow
net.get("http://example.com")      # HTTP GET request
net.post("http://example.com", data) # HTTP POST request
```

## AI Module (Planned)

The AI module will provide AI-specific functions.

### Functions (Planned)
```synthflow
ai.intent("Calculate the sum of 2 and 3")  # Intent recognition
ai.generate("Generate a poem about spring") # Text generation
```