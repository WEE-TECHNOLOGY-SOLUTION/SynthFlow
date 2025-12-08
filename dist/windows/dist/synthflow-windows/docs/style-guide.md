# Style Guide

This document outlines the coding conventions and style guidelines for SynthFlow development, including both the language itself and its implementation.

## Overview

Consistent coding style improves readability, maintainability, and collaboration. This guide covers formatting, naming conventions, documentation, and best practices for SynthFlow code and compiler implementation.

## Language Style Guide

### Formatting

#### Indentation
- Use 4 spaces for indentation (no tabs)
- Maintain consistent indentation levels
- Align continuation lines with opening delimiters

```synthflow
// Good
fn calculate_sum(numbers: Array<Int>) -> Int {
    let sum = 0
    for number in numbers {
        sum = sum + number
    }
    return sum
}

// Avoid
fn calculate_sum(numbers: Array<Int>) -> Int {
  let sum = 0
  for number in numbers {
    sum = sum + number
  }
  return sum
}
```

#### Line Length
- Limit lines to 100 characters
- Break long lines at logical points
- Use hanging indents for continuation lines

```synthflow
// Good
let very_long_variable_name = some_function(
    first_parameter,
    second_parameter,
    third_parameter
)

// Avoid
let very_long_variable_name = some_function(first_parameter, second_parameter, third_parameter)
```

#### Whitespace
- Use spaces around operators
- No trailing whitespace
- Use blank lines to separate logical sections
- One blank line between functions

```synthflow
// Good
let x = 5 + 3
let y = x * 2

fn add(a: Int, b: Int) -> Int {
    return a + b
}

fn multiply(a: Int, b: Int) -> Int {
    return a * b
}

// Avoid
let x=5+3
let y=x*2
fn add(a:Int,b:Int)->Int{return a+b}
fn multiply(a:Int,b:Int)->Int{return a*b}
```

#### Braces and Blocks
- Opening brace on same line as declaration
- Closing brace on its own line
- Always use braces for control structures

```synthflow
// Good
if condition {
    do_something()
} else {
    do_something_else()
}

// Avoid
if condition
{
    do_something()
}
else
{
    do_something_else()
}
```

### Naming Conventions

#### Variables and Functions
- Use `snake_case` for variables and functions
- Use descriptive names that convey purpose
- Avoid abbreviations unless widely understood

```synthflow
// Good
let user_count = 0
let max_retries = 3
fn calculate_average(values: Array<Float>) -> Float { }

// Avoid
let usr_cnt = 0
let mx_rtrs = 3
fn calc_avg(vals: Array<Float>) -> Float { }
```

#### Constants
- Use `UPPER_SNAKE_CASE` for constants
- Prefix with `k` for module-level constants

```synthflow
// Good
const MAX_BUFFER_SIZE = 1024
const kDefaultTimeout = 30

// Avoid
const max_buffer_size = 1024
const defaultTimeout = 30
```

#### Types and Classes
- Use `PascalCase` for types and classes
- Use singular names for classes
- Use descriptive names

```synthflow
// Good
struct User {
    name: String
    age: Int
}

type Result<T, E> = Ok(T) | Err(E)

// Avoid
struct user {
    name: String
    age: Int
}

type result<T, E> = Ok(T) | Err(E)
```

#### Enums
- Use `PascalCase` for enum names
- Use `PascalCase` for enum variants

```synthflow
// Good
enum Color {
    Red
    Green
    Blue
}

// Avoid
enum color {
    red
    green
    blue
}
```

### Comments and Documentation

#### Inline Comments
- Use `#` for single-line comments
- Place comments above the code they describe
- Use clear, concise language
- Focus on why, not what

```synthflow
// Good
# Calculate the fibonacci sequence up to n terms
# This implementation uses an iterative approach for efficiency
fn fibonacci(n: Int) -> Array<Int> {
    # Handle edge cases
    if n <= 0 {
        return []
    }
    
    let sequence = [1, 1]
    # Generate remaining terms
    for i in 2..n {
        sequence.push(sequence[i-1] + sequence[i-2])
    }
    return sequence
}

// Avoid
fn fibonacci(n: Int) -> Array<Int> {
    if n <= 0 {  # check if n is less than or equal to 0
        return []
    }
    
    let sequence = [1, 1]  # initialize with first two terms
    for i in 2..n {  # loop from 2 to n
        sequence.push(sequence[i-1] + sequence[i-2])  # add previous two terms
    }
    return sequence
}
```

#### Documentation Comments
- Use triple-hash `###` for documentation comments
- Document all public functions and types
- Include examples when helpful
- Follow standard documentation format

```synthflow
###
Calculate the factorial of a number.
    
Parameters:
    n: The number to calculate factorial for
    
Returns:
    The factorial of n
    
Example:
    factorial(5)  # Returns 120
###
fn factorial(n: Int) -> Int {
    if n <= 1 {
        return 1
    }
    return n * factorial(n - 1)
}
```

### Control Structures

#### If Statements
- Use explicit boolean expressions
- Avoid complex conditions in if statements
- Consider pattern matching for complex conditions

```synthflow
// Good
if user.is_authenticated() && user.has_permission("read") {
    show_content()
}

// Avoid
if user.is_authenticated() && user.has_permission("read") && user.profile.is_complete() && user.subscription.is_active() {
    show_content()
}
```

#### Loops
- Use appropriate loop constructs for the task
- Prefer higher-order functions when applicable
- Use descriptive iterator names

```synthflow
// Good
for item in items {
    process(item)
}

for i in 0..10 {
    println("Count: " + i)
}

items.foreach(process)

// Avoid
let i = 0
while i < items.length() {
    process(items[i])
    i = i + 1
}
```

#### Pattern Matching
- Use pattern matching for complex conditions
- Exhaust all cases when possible
- Use wildcard patterns appropriately

```synthflow
// Good
match result {
    Ok(value) => println("Success: " + value)
    Err(error) => println("Error: " + error)
}

// Avoid
if result.is_ok() {
    println("Success: " + result.unwrap())
} else {
    println("Error: " + result.unwrap_err())
}
```

### Error Handling

#### Result Types
- Prefer Result types for recoverable errors
- Use descriptive error types
- Handle all possible error cases

```synthflow
// Good
fn divide(a: Float, b: Float) -> Result<Float, String> {
    if b == 0.0 {
        return Err("Division by zero")
    }
    return Ok(a / b)
}

match divide(10.0, 2.0) {
    Ok(result) => println("Result: " + result)
    Err(error) => println("Error: " + error)
}

// Avoid
fn divide(a: Float, b: Float) -> Float {
    if b == 0.0 {
        panic("Division by zero")
    }
    return a / b
}
```

### Functions

#### Function Length
- Keep functions short and focused
- Aim for functions that do one thing well
- Break complex functions into smaller helpers

#### Parameters
- Limit function parameters to 5 or fewer
- Use structs for related parameters
- Prefer named parameters for complex functions

```synthflow
// Good
struct DatabaseConfig {
    host: String
    port: Int
    username: String
    password: String
}

fn connect_to_database(config: DatabaseConfig) -> DatabaseConnection {
    # Implementation
}

// Avoid
fn connect_to_database(host: String, port: Int, username: String, password: String, timeout: Int, retries: Int) -> DatabaseConnection {
    # Implementation
}
```

## Compiler Implementation Style Guide

### C++ Coding Standards

#### General Principles
- Follow the Google C++ Style Guide with SynthFlow-specific exceptions
- Use `std::unique_ptr` for automatic memory management
- Prefer RAII over manual resource management
- Use const correctness extensively

#### Naming Conventions
- Classes: `PascalCase`
- Functions: `camelCase`
- Variables: `snake_case`
- Constants: `kPascalCase`
- Macros: `UPPER_CASE`

#### File Organization
- Header files: `.h`
- Source files: `.cpp`
- One class per header file when possible
- Inline functions in headers only when necessary

#### Include Guards
```cpp
// Good
#pragma once

// Avoid
#ifndef SYNTHFLOW_TOKEN_H
#define SYNTHFLOW_TOKEN_H
// ... content ...
#endif
```

#### Class Design
- Use the rule of five (or zero) when appropriate
- Prefer composition over inheritance
- Use explicit constructors
- Mark overriding functions with `override`

```cpp
// Good
class Lexer {
public:
    explicit Lexer(std::string source);
    
    std::vector<Token> tokenize();
    
private:
    std::string source_;
    size_t pos_;
};

// Avoid
class Lexer {
public:
    Lexer(std::string source) : source(source), pos(0) {}
    
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos;
};
```

#### Memory Management
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Avoid raw `new` and `delete`
- Use `std::make_unique` and `std::make_shared`
- Prefer stack allocation when possible

#### Error Handling
- Use exceptions for exceptional circumstances
- Use `std::expected` or similar for expected errors
- Provide meaningful error messages
- Include source location information when appropriate

### Code Documentation

#### Comments
- Comment complex algorithms and decisions
- Explain why, not what
- Keep comments up to date with code changes
- Remove outdated comments

#### Documentation
- Document all public APIs
- Use Doxygen-style comments for C++ code
- Include examples in documentation
- Document preconditions and postconditions

```cpp
/**
 * Tokenize the source code into a stream of tokens.
 * 
 * This function performs lexical analysis on the source code,
 * converting it into a sequence of tokens that can be parsed
 * by the parser.
 * 
 * @return Vector of tokens representing the source code
 * @throws LexerError if invalid syntax is encountered
 */
std::vector<Token> Lexer::tokenize();
```

### Testing Standards

#### Test Naming
- Use descriptive test names
- Follow the pattern: `TestSuite_TestName`
- Include expected behavior in test names

```cpp
// Good
TEST(LexerTest, TokenizesSimpleFunctionDeclaration)
TEST(ParserTest, HandlesNestedFunctionCalls)
TEST(SemanticAnalyzerTest, DetectsTypeMismatchInAssignment)

// Avoid
TEST(LexerTest, Test1)
TEST(ParserTest, TestNestedCalls)
TEST(SemanticAnalyzerTest, TestTypeMismatch)
```

#### Test Structure
- Arrange-Act-Assert pattern
- One assertion per test when possible
- Use meaningful test data
- Clean up test resources

```cpp
TEST(LexerTest, TokenizesIntegerLiterals) {
    // Arrange
    std::string source = "42 123 999";
    Lexer lexer(source);
    
    // Act
    auto tokens = lexer.tokenize();
    
    // Assert
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER);
    EXPECT_EQ(std::get<int64_t>(tokens[0].value), 42);
    EXPECT_EQ(tokens[1].type, TokenType::INTEGER);
    EXPECT_EQ(std::get<int64_t>(tokens[1].value), 123);
    EXPECT_EQ(tokens[2].type, TokenType::INTEGER);
    EXPECT_EQ(std::get<int64_t>(tokens[2].value), 999);
}
```

## Tools and Automation

### Code Formatting
- Use automated formatters (clang-format for C++)
- Configure editor to format on save
- Include formatting checks in CI pipeline

### Static Analysis
- Use static analysis tools (clang-tidy, etc.)
- Address all high-priority warnings
- Configure analysis in CI pipeline

### Linting
- Use linters appropriate to each language
- Configure editor integration
- Include linting in CI pipeline

## Review Process

### Code Reviews
- All code changes require review
- Focus on correctness, readability, and maintainability
- Provide constructive feedback
- Follow up on review comments

### Documentation Reviews
- Documentation changes require review
- Check for accuracy and clarity
- Verify examples work correctly
- Ensure consistent style

## Evolution

This style guide will evolve as the project grows. Major changes will be communicated to the community, and updates will be made with careful consideration of existing code and developer productivity.

For questions about this style guide or to propose changes, please open an issue in our GitHub repository.