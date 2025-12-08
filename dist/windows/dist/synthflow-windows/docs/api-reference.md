# API Reference

This document provides a comprehensive reference for the SynthFlow standard library and built-in functions, organized by module and functionality.

## Overview

The SynthFlow standard library provides essential functionality for common programming tasks, including data structures, I/O operations, string manipulation, mathematical operations, and system interfaces.

## Core Types

### Primitive Types

#### Int
64-bit signed integer type.

```synthflow
let value: Int = 42
let max_int: Int = 9223372036854775807
let min_int: Int = -9223372036854775808
```

##### Methods
- `to_string() -> String`: Convert to string representation
- `to_float() -> Float`: Convert to floating-point number
- `abs() -> Int`: Absolute value
- `min(other: Int) -> Int`: Minimum of two values
- `max(other: Int) -> Int`: Maximum of two values

#### Float
64-bit floating-point number type.

```synthflow
let value: Float = 3.14159
let max_float: Float = 1.7976931348623157e+308
let min_float: Float = 2.2250738585072014e-308
```

##### Methods
- `to_string() -> String`: Convert to string representation
- `to_int() -> Int`: Convert to integer (truncates)
- `abs() -> Float`: Absolute value
- `ceil() -> Float`: Ceiling value
- `floor() -> Float`: Floor value
- `round() -> Float`: Rounded value
- `sqrt() -> Float`: Square root
- `pow(exp: Float) -> Float`: Raise to power
- `sin() -> Float`: Sine function
- `cos() -> Float`: Cosine function
- `tan() -> Float`: Tangent function
- `log() -> Float`: Natural logarithm
- `log10() -> Float`: Base-10 logarithm

#### Bool
Boolean type with values `true` and `false`.

```synthflow
let is_true: Bool = true
let is_false: Bool = false
```

##### Methods
- `to_string() -> String`: Convert to string ("true" or "false")
- `not() -> Bool`: Logical negation

#### String
Immutable sequence of characters.

```synthflow
let text: String = "Hello, World!"
let multiline: String = """
This is a
multiline string
"""
```

##### Methods
- `length() -> Int`: Number of characters
- `is_empty() -> Bool`: Check if empty
- `contains(substring: String) -> Bool`: Check if contains substring
- `starts_with(prefix: String) -> Bool`: Check if starts with prefix
- `ends_with(suffix: String) -> Bool`: Check if ends with suffix
- `to_uppercase() -> String`: Convert to uppercase
- `to_lowercase() -> String`: Convert to lowercase
- `trim() -> String`: Remove whitespace from both ends
- `split(delimiter: String) -> Array<String>`: Split into array
- `replace(from: String, to: String) -> String`: Replace substring
- `substring(start: Int, end: Int) -> String`: Extract substring
- `char_at(index: Int) -> Char`: Get character at index
- `to_int() -> Result<Int, String>`: Parse as integer
- `to_float() -> Result<Float, String>`: Parse as float

#### Char
Single Unicode character.

```synthflow
let letter: Char = 'A'
let digit: Char = '7'
let emoji: Char = '🚀'
```

##### Methods
- `to_string() -> String`: Convert to string
- `is_digit() -> Bool`: Check if digit
- `is_letter() -> Bool`: Check if letter
- `is_whitespace() -> Bool`: Check if whitespace
- `to_uppercase() -> Char`: Convert to uppercase
- `to_lowercase() -> Char`: Convert to lowercase

### Collection Types

#### Array<T>
Resizable array of elements of type T.

```synthflow
let numbers: Array<Int> = [1, 2, 3, 4, 5]
let empty: Array<String> = []
let initialized: Array<Int> = Array(10, 0)  // 10 elements, all 0
```

##### Methods
- `length() -> Int`: Number of elements
- `is_empty() -> Bool`: Check if empty
- `get(index: Int) -> T`: Get element at index (panics if out of bounds)
- `get_safe(index: Int) -> Option<T>`: Get element safely
- `set(index: Int, value: T) -> Void`: Set element at index
- `push(value: T) -> Void`: Add element to end
- `pop() -> Option<T>`: Remove and return last element
- `insert(index: Int, value: T) -> Void`: Insert element at index
- `remove(index: Int) -> T`: Remove and return element at index
- `clear() -> Void`: Remove all elements
- `contains(value: T) -> Bool`: Check if contains value
- `index_of(value: T) -> Option<Int>`: Find index of value
- `slice(start: Int, end: Int) -> Array<T>`: Create sub-array
- `map<U>(f: Fn(T) -> U) -> Array<U>`: Transform elements
- `filter(f: Fn(T) -> Bool) -> Array<T>`: Filter elements
- `fold<U>(initial: U, f: Fn(U, T) -> U) -> U`: Reduce to single value
- `foreach(f: Fn(T) -> Void) -> Void`: Apply function to each element
- `sort() -> Array<T>`: Return sorted copy
- `sort_by(f: Fn(T, T) -> Int) -> Array<T>`: Sort with custom comparator

#### List<T>
Linked list of elements of type T.

```synthflow
let numbers: List<Int> = List(1, 2, 3, 4, 5)
let empty: List<String> = List()
```

##### Methods
- `length() -> Int`: Number of elements
- `is_empty() -> Bool`: Check if empty
- `push_front(value: T) -> Void`: Add element to front
- `push_back(value: T) -> Void`: Add element to back
- `pop_front() -> Option<T>`: Remove and return first element
- `pop_back() -> Option<T>`: Remove and return last element
- `front() -> Option<T>`: Get first element
- `back() -> Option<T>`: Get last element
- `contains(value: T) -> Bool`: Check if contains value
- `foreach(f: Fn(T) -> Void) -> Void`: Apply function to each element

#### Map<K, V>
Hash map with keys of type K and values of type V.

```synthflow
let ages: Map<String, Int> = Map()
ages.insert("Alice", 30)
ages.insert("Bob", 25)

let initialized: Map<String, Int> = Map([
    ("Alice", 30),
    ("Bob", 25)
])
```

##### Methods
- `length() -> Int`: Number of key-value pairs
- `is_empty() -> Bool`: Check if empty
- `insert(key: K, value: V) -> Void`: Insert key-value pair
- `get(key: K) -> Option<V>`: Get value for key
- `remove(key: K) -> Option<V>`: Remove and return value for key
- `contains(key: K) -> Bool`: Check if contains key
- `keys() -> Array<K>`: Get all keys
- `values() -> Array<V>`: Get all values
- `foreach(f: Fn(K, V) -> Void) -> Void`: Apply function to each pair
- `clear() -> Void`: Remove all key-value pairs

#### Set<T>
Hash set of elements of type T.

```synthflow
let numbers: Set<Int> = Set()
numbers.insert(1)
numbers.insert(2)
numbers.insert(3)

let initialized: Set<Int> = Set([1, 2, 3, 4, 5])
```

##### Methods
- `length() -> Int`: Number of elements
- `is_empty() -> Bool`: Check if empty
- `insert(value: T) -> Void`: Add element
- `remove(value: T) -> Bool`: Remove element, return true if existed
- `contains(value: T) -> Bool`: Check if contains element
- `foreach(f: Fn(T) -> Void) -> Void`: Apply function to each element
- `clear() -> Void`: Remove all elements
- `union(other: Set<T>) -> Set<T>`: Union with another set
- `intersection(other: Set<T>) -> Set<T>`: Intersection with another set
- `difference(other: Set<T>) -> Set<T>`: Difference with another set

## I/O Module

### Console I/O

#### print
Print value to standard output without newline.

```synthflow
fn print(value: Any) -> Void
```

#### println
Print value to standard output with newline.

```synthflow
fn println(value: Any) -> Void
```

#### read_line
Read line from standard input.

```synthflow
fn read_line() -> String
```

#### read_char
Read single character from standard input.

```synthflow
fn read_char() -> Char
```

### File I/O

#### File
File handle for reading and writing.

```synthflow
struct File {
    // Private fields
}
```

##### Methods
- `open(path: String, mode: FileMode) -> Result<File, IOError>`: Open file
- `read() -> Result<String, IOError>`: Read entire file
- `read_line() -> Result<String, IOError>`: Read line
- `write(data: String) -> Result<Void, IOError>`: Write data
- `flush() -> Result<Void, IOError>`: Flush buffered data
- `close() -> Result<Void, IOError>`: Close file

#### FileMode
Enumeration of file open modes.

```synthflow
enum FileMode {
    Read
    Write
    Append
    ReadWrite
}
```

#### Functions
- `read_file(path: String) -> Result<String, IOError>`: Read entire file
- `write_file(path: String, content: String) -> Result<Void, IOError>`: Write file
- `append_file(path: String, content: String) -> Result<Void, IOError>`: Append to file
- `exists(path: String) -> Bool`: Check if file exists
- `is_file(path: String) -> Bool`: Check if path is file
- `is_directory(path: String) -> Bool`: Check if path is directory
- `create_directory(path: String) -> Result<Void, IOError>`: Create directory
- `remove_file(path: String) -> Result<Void, IOError>`: Remove file
- `remove_directory(path: String) -> Result<Void, IOError>`: Remove directory

## Math Module

### Constants
- `PI: Float = 3.141592653589793`: Mathematical constant π
- `E: Float = 2.718281828459045`: Mathematical constant e
- `TAU: Float = 6.283185307179586`: Mathematical constant τ (2π)

### Functions
- `abs<T: Numeric>(value: T) -> T`: Absolute value
- `min<T: Numeric>(a: T, b: T) -> T`: Minimum of two values
- `max<T: Numeric>(a: T, b: T) -> T`: Maximum of two values
- `clamp<T: Numeric>(value: T, min: T, max: T) -> T`: Clamp value to range
- `sign<T: Numeric>(value: T) -> Int`: Sign of value (-1, 0, or 1)
- `sqrt(value: Float) -> Float`: Square root
- `pow(base: Float, exp: Float) -> Float`: Raise to power
- `exp(value: Float) -> Float`: Exponential function
- `log(value: Float) -> Float`: Natural logarithm
- `log10(value: Float) -> Float`: Base-10 logarithm
- `log2(value: Float) -> Float`: Base-2 logarithm
- `sin(value: Float) -> Float`: Sine function
- `cos(value: Float) -> Float`: Cosine function
- `tan(value: Float) -> Float`: Tangent function
- `asin(value: Float) -> Float`: Arcsine function
- `acos(value: Float) -> Float`: Arccosine function
- `atan(value: Float) -> Float`: Arctangent function
- `atan2(y: Float, x: Float) -> Float`: Arctangent of y/x
- `sinh(value: Float) -> Float`: Hyperbolic sine
- `cosh(value: Float) -> Float`: Hyperbolic cosine
- `tanh(value: Float) -> Float`: Hyperbolic tangent
- `ceil(value: Float) -> Float`: Ceiling function
- `floor(value: Float) -> Float`: Floor function
- `round(value: Float) -> Float`: Round to nearest integer
- `trunc(value: Float) -> Float`: Truncate toward zero
- `fract(value: Float) -> Float`: Fractional part
- `gcd(a: Int, b: Int) -> Int`: Greatest common divisor
- `lcm(a: Int, b: Int) -> Int`: Least common multiple

## Random Module

### Functions
- `random() -> Float`: Generate random float between 0.0 and 1.0
- `random_int(max: Int) -> Int`: Generate random integer between 0 and max-1
- `random_range(min: Int, max: Int) -> Int`: Generate random integer in range
- `random_choice<T>(array: Array<T>) -> T`: Choose random element from array
- `shuffle<T>(array: Array<T>) -> Array<T>`: Return shuffled copy of array

### Random
Random number generator.

```synthflow
struct Random {
    // Private fields
}
```

##### Methods
- `new(seed: Int) -> Random`: Create new random number generator
- `next_float() -> Float`: Generate next random float
- `next_int(max: Int) -> Int`: Generate next random integer
- `next_range(min: Int, max: Int) -> Int`: Generate random integer in range

## Time Module

### DateTime
Date and time representation.

```synthflow
struct DateTime {
    year: Int
    month: Int
    day: Int
    hour: Int
    minute: Int
    second: Int
    millisecond: Int
}
```

##### Methods
- `now() -> DateTime`: Get current date and time
- `from_timestamp(timestamp: Int) -> DateTime`: Create from Unix timestamp
- `to_timestamp() -> Int`: Convert to Unix timestamp
- `format(format: String) -> String`: Format as string
- `add_days(days: Int) -> DateTime`: Add days
- `add_hours(hours: Int) -> DateTime`: Add hours
- `add_minutes(minutes: Int) -> DateTime`: Add minutes
- `add_seconds(seconds: Int) -> DateTime`: Add seconds
- `difference(other: DateTime) -> Duration`: Difference between dates

### Duration
Time duration representation.

```synthflow
struct Duration {
    days: Int
    hours: Int
    minutes: Int
    seconds: Int
    milliseconds: Int
}
```

##### Methods
- `from_seconds(seconds: Int) -> Duration`: Create from seconds
- `total_seconds() -> Int`: Total duration in seconds
- `format() -> String`: Format as human-readable string

### Functions
- `sleep(milliseconds: Int) -> Void`: Sleep for specified milliseconds
- `measure_time<F>(f: F) -> (Duration, Result<T, E>)`: Measure execution time

## System Module

### Environment
Environment variables and process information.

##### Methods
- `env(key: String) -> Option<String>`: Get environment variable
- `set_env(key: String, value: String) -> Void`: Set environment variable
- `args() -> Array<String>`: Get command-line arguments
- `current_dir() -> String`: Get current working directory
- `set_current_dir(path: String) -> Result<Void, IOError>`: Set current directory
- `exit(code: Int) -> Void`: Exit program with code
- `pid() -> Int`: Get process ID

### Process
Process management.

```synthflow
struct Process {
    // Private fields
}
```

##### Methods
- `run(command: String) -> Result<ProcessResult, ProcessError>`: Run command
- `spawn(command: String) -> Result<Process, ProcessError>`: Spawn process
- `wait() -> Result<ProcessResult, ProcessError>`: Wait for completion

## Networking Module

### HTTP
HTTP client and server functionality.

#### HttpClient
HTTP client for making requests.

```synthflow
struct HttpClient {
    // Private fields
}
```

##### Methods
- `new() -> HttpClient`: Create new HTTP client
- `get(url: String) -> Result<HttpResponse, HttpError>`: Make GET request
- `post(url: String, body: String) -> Result<HttpResponse, HttpError>`: Make POST request
- `put(url: String, body: String) -> Result<HttpResponse, HttpError>`: Make PUT request
- `delete(url: String) -> Result<HttpResponse, HttpError>`: Make DELETE request

#### HttpResponse
HTTP response.

```synthflow
struct HttpResponse {
    status: Int
    headers: Map<String, String>
    body: String
}
```

##### Methods
- `json<T>() -> Result<T, JsonError>`: Parse response as JSON
- `text() -> String`: Get response body as text

#### HttpServer
HTTP server for handling requests.

```synthflow
struct HttpServer {
    // Private fields
}
```

##### Methods
- `new(port: Int) -> HttpServer`: Create new HTTP server
- `get(path: String, handler: Fn(HttpRequest) -> HttpResponse) -> Void`: Handle GET requests
- `post(path: String, handler: Fn(HttpRequest) -> HttpResponse) -> Void`: Handle POST requests
- `start() -> Result<Void, HttpError>`: Start server
- `stop() -> Void`: Stop server

## Concurrency Module

### Thread
Thread for parallel execution.

```synthflow
struct Thread {
    // Private fields
}
```

##### Methods
- `spawn<F>(f: F) -> Thread`: Spawn new thread
- `join() -> Result<Void, ThreadError>`: Wait for thread completion
- `sleep(milliseconds: Int) -> Void`: Sleep current thread

### Mutex<T>
Mutual exclusion lock.

```synthflow
struct Mutex<T> {
    // Private fields
}
```

##### Methods
- `new(value: T) -> Mutex<T>`: Create new mutex
- `lock() -> MutexGuard<T>`: Acquire lock
- `try_lock() -> Option<MutexGuard<T>>`: Try to acquire lock

### Channel<T>
Channel for message passing between threads.

```synthflow
struct Channel<T> {
    // Private fields
}
```

##### Methods
- `new() -> (Sender<T>, Receiver<T>)`: Create new channel
- `send(value: T) -> Result<Void, ChannelError>`: Send value
- `recv() -> Result<T, ChannelError>`: Receive value

## JSON Module

### Functions
- `parse<T>(json: String) -> Result<T, JsonError>`: Parse JSON string
- `stringify<T>(value: T) -> String`: Convert value to JSON string

### JsonValue
Generic JSON value representation.

```synthflow
enum JsonValue {
    Null
    Bool(Bool)
    Number(Float)
    String(String)
    Array(Array<JsonValue>)
    Object(Map<String, JsonValue>)
}
```

## Testing Module

### Functions
- `assert(condition: Bool, message: String) -> Void`: Assert condition is true
- `assert_equal<T>(actual: T, expected: T) -> Void`: Assert values are equal
- `assert_not_equal<T>(actual: T, expected: T) -> Void`: Assert values are not equal
- `fail(message: String) -> Void`: Fail test with message

### TestResult
Result of test execution.

```synthflow
enum TestResult {
    Passed
    Failed(String)
    Skipped
}
```

## Error Types

### IOError
Input/output error.

```synthflow
enum IOError {
    FileNotFound(String)
    PermissionDenied(String)
    InvalidData(String)
    UnexpectedEof
    Other(String)
}
```

### HttpError
HTTP-related error.

```synthflow
enum HttpError {
    ConnectionFailed(String)
    Timeout
    InvalidUrl(String)
    InvalidResponse(String)
    Other(String)
}
```

### JsonError
JSON parsing error.

```synthflow
enum JsonError {
    InvalidSyntax(String)
    TypeMismatch(String)
    MissingField(String)
    Other(String)
}
```

### ProcessError
Process execution error.

```synthflow
enum ProcessError {
    NotFound(String)
    PermissionDenied(String)
    ExitCode(Int)
    Other(String)
}
```

This API reference provides documentation for the core SynthFlow standard library. For the most current information, refer to the inline documentation in the standard library source code.