<![CDATA[<div align="center">

# 📘 SynthFlow Programming Language

## Zero to Hero

### A Complete Guide to Modern AI-Native Programming

---

**Version 0.0.30**

**Author:** WEE Technology Solution

**Published:** December 2025

---

*Learn to build powerful applications with the world's first AI-native programming language*

</div>

---

# Table of Contents

1. [Introduction to SynthFlow](#chapter-1-introduction-to-synthflow)
2. [Getting Started](#chapter-2-getting-started)
3. [Basic Syntax & Variables](#chapter-3-basic-syntax--variables)
4. [Control Flow](#chapter-4-control-flow)
5. [Functions & Lambdas](#chapter-5-functions--lambdas)
6. [Data Structures](#chapter-6-data-structures)
7. [Object-Oriented Programming](#chapter-7-object-oriented-programming)
8. [Error Handling](#chapter-8-error-handling)
9. [File I/O & System Operations](#chapter-9-file-io--system-operations)
10. [Web Development](#chapter-10-web-development)
11. [AI Integration](#chapter-11-ai-integration)
12. [Quantum Computing](#chapter-12-quantum-computing)
13. [Machine Learning & Data Science](#chapter-13-machine-learning--data-science)
14. [Best Practices & Projects](#chapter-14-best-practices--projects)
- [Appendix A: Quick Reference](#appendix-a-quick-reference)
- [Appendix B: Standard Library](#appendix-b-standard-library)
- [Appendix C: Resources](#appendix-c-resources)

---

# Part I: Foundations

---

# Chapter 1: Introduction to SynthFlow

## What is SynthFlow?

**SynthFlow** is a revolutionary **AI-native programming language** designed to bridge the gap between human intent and machine execution. It combines the readability of Python, the type safety of TypeScript, and adds cutting-edge features like native AI integration and quantum computing support.

## Why Choose SynthFlow?

| Feature | SynthFlow | Python | JavaScript |
|---------|-----------|--------|------------|
| Clean Syntax | ✅ | ✅ | ❌ |
| Type Safety | ✅ Optional | ❌ | ❌ |
| Native AI Integration | ✅ Built-in | ❌ Libraries | ❌ Libraries |
| Quantum Computing | ✅ Native | ❌ Libraries | ❌ |
| Web Framework | ✅ Built-in | ❌ Flask/Django | ✅ |
| Compilation Speed | ✅ Fast | ❌ Interpreted | ❌ Interpreted |

## Key Features

### 🧠 AI-Native Design
SynthFlow was built from the ground up with AI in mind. It includes native support for:
- Google Gemini API integration
- Agent development framework (SADK)
- Vector stores for RAG applications

### ⚛️ Quantum Computing
Simulate quantum algorithms directly in SynthFlow:
- Qubit state simulation
- Quantum gates (Hadamard, CNOT, Pauli)
- Entanglement and superposition

### 🌐 Web Development
Build web applications with minimal code:
- One-liner route definitions
- Built-in HTTP server
- JSON/HTML/Text response builders

### 📱 Mobile Development
Create mobile apps that compile to:
- React Native for iOS/Android
- Progressive Web Apps (PWA)

## SynthFlow Ecosystem

```
┌─────────────────────────────────────────────────────────┐
│                    SynthFlow Ecosystem                   │
├─────────────────────────────────────────────────────────┤
│  🖥️ CLI Tools        │  📦 Package Manager  │  🔌 IDE   │
│  - synthflow run     │  - synthpkg          │  - VS Code│
│  - synthflow repl    │                      │  - LSP    │
│  - synthflow check   │                      │  - MCP    │
├─────────────────────────────────────────────────────────┤
│               📚 Standard Library (40+ Modules)          │
│  AI | HTTP | JSON | Math | Quantum | Neural Network...  │
└─────────────────────────────────────────────────────────┘
```

---

# Chapter 2: Getting Started

## Installation

### Windows

1. **Download** the latest release from GitHub:
   ```
   https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow/releases
   ```

2. **Run** the installer (`synthflow-0.0.30.exe`)

3. **Verify** installation:
   ```bash
   synthflow --version
   ```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow.git
cd synthflow

# Build on Windows
.\build.bat

# Build on Linux/macOS
./build.sh
```

## Setting Up VS Code

1. Install the **SynthFlow** extension from VS Code marketplace
2. The extension provides:
   - Syntax highlighting for `.sf` files
   - Code completion via LSP
   - Integrated debugging

## Your First Program

Create a file named `hello.sf`:

```synthflow
# My first SynthFlow program
let message = "Hello, SynthFlow!"
print(message)
```

Run it:
```bash
synthflow run hello.sf
```

**Output:**
```
Hello, SynthFlow!
```

## Using the REPL

SynthFlow includes an interactive REPL for quick experimentation:

```bash
synthflow repl
```

```
SynthFlow REPL v0.0.30
Type 'exit' to quit, 'help' for commands

>>> let x = 10
>>> let y = 20
>>> print(x + y)
30
>>> exit
```

## CLI Commands

| Command | Description |
|---------|-------------|
| `synthflow run <file>` | Run a SynthFlow program |
| `synthflow repl` | Start interactive REPL |
| `synthflow check <file>` | Type-check without running |
| `synthflow -c "code"` | Execute inline code |
| `synthflow -m <module>` | Run a module |
| `synthflow --help` | Show all commands |

---

# Chapter 3: Basic Syntax & Variables

## Variables

SynthFlow uses `let` for mutable variables and `const` for immutable values:

```synthflow
# Mutable variable
let count = 0
count = count + 1  # OK

# Immutable constant
const PI = 3.14159
PI = 3.14  # ERROR! Cannot reassign const
```

## Data Types

### Primitives

```synthflow
# Integer (64-bit signed)
let age = 25
let negative = -100

# Float (64-bit)
let price = 19.99
let scientific = 1.5e10

# String
let name = "Alice"
let multiline = """
This is a
multiline string
"""

# Boolean
let is_active = true
let is_admin = false
```

### Type Annotations

SynthFlow supports optional type annotations:

```synthflow
let x: Int = 42
let y: Float = 3.14
let name: String = "Bob"
let flag: Bool = true
```

## Operators

### Arithmetic Operators

```synthflow
let a = 10
let b = 3

print(a + b)   # Addition: 13
print(a - b)   # Subtraction: 7
print(a * b)   # Multiplication: 30
print(a / b)   # Division: 3.333...
print(a % b)   # Modulo: 1
```

### Comparison Operators

```synthflow
print(5 > 3)   # true
print(5 < 3)   # false
print(5 >= 5)  # true
print(5 <= 4)  # false
print(5 == 5)  # true
print(5 != 3)  # true
```

### Logical Operators

```synthflow
let a = true
let b = false

print(a && b)  # AND: false
print(a || b)  # OR: true
print(!a)      # NOT: false
```

### Compound Assignment

```synthflow
let x = 10
x += 5   # x = 15
x -= 3   # x = 12
x *= 2   # x = 24
x /= 4   # x = 6
```

### Increment/Decrement

```synthflow
let i = 0
i++  # i = 1
i--  # i = 0
```

## String Operations

### Concatenation

```synthflow
let first = "Hello"
let last = "World"
let full = first + ", " + last + "!"
print(full)  # "Hello, World!"
```

### String Interpolation

```synthflow
let name = "Alice"
let age = 25
let message = "My name is ${name} and I am ${age} years old."
print(message)
```

### String Functions

```synthflow
let text = "  Hello World  "
print(len(text))           # 15 (length)
print(str(42))             # "42" (convert to string)
```

## Comments

```synthflow
# This is a single-line comment

// This is also a single-line comment

/* This is a
   multi-line comment */
```

## Built-in Functions

| Function | Description | Example |
|----------|-------------|---------|
| `print(...)` | Print to console | `print("Hello")` |
| `len(x)` | Get length | `len([1,2,3])` → `3` |
| `str(x)` | Convert to string | `str(42)` → `"42"` |
| `int(x)` | Convert to integer | `int("42")` → `42` |
| `float(x)` | Convert to float | `float("3.14")` → `3.14` |
| `typeof(x)` | Get type name | `typeof(42)` → `"Int"` |

---

# Chapter 4: Control Flow

## If/Else Statements

```synthflow
let score = 85

if score >= 90 {
    print("Grade: A")
} else if score >= 80 {
    print("Grade: B")
} else if score >= 70 {
    print("Grade: C")
} else {
    print("Grade: F")
}
```

### Parentheses are Optional

```synthflow
# Both are valid:
if (score > 50) {
    print("Pass")
}

if score > 50 {
    print("Pass")
}
```

## While Loops

```synthflow
let count = 0

while count < 5 {
    print("Count:", count)
    count = count + 1
}
```

**Output:**
```
Count: 0
Count: 1
Count: 2
Count: 3
Count: 4
```

## For Loops

### Traditional For Loop

```synthflow
for (let i = 0; i < 5; i++) {
    print("Iteration:", i)
}
```

### For Loop with Array

```synthflow
let fruits = ["apple", "banana", "cherry"]

for (let i = 0; i < len(fruits); i++) {
    print(fruits[i])
}
```

## Break and Continue

### Break - Exit Loop Early

```synthflow
let i = 0
while i < 10 {
    if i == 5 {
        break  # Exit when i reaches 5
    }
    print(i)
    i++
}
# Output: 0, 1, 2, 3, 4
```

### Continue - Skip Iteration

```synthflow
for (let i = 0; i < 5; i++) {
    if i == 2 {
        continue  # Skip when i is 2
    }
    print(i)
}
# Output: 0, 1, 3, 4
```

## Nested Control Structures

```synthflow
# Print a multiplication table
for (let i = 1; i <= 5; i++) {
    for (let j = 1; j <= 5; j++) {
        let product = i * j
        print(str(i) + " x " + str(j) + " = " + str(product))
    }
    print("---")
}
```

## Practical Example: FizzBuzz

```synthflow
# Classic FizzBuzz problem
for (let i = 1; i <= 20; i++) {
    if i % 15 == 0 {
        print("FizzBuzz")
    } else if i % 3 == 0 {
        print("Fizz")
    } else if i % 5 == 0 {
        print("Buzz")
    } else {
        print(i)
    }
}
```

---

# Chapter 5: Functions & Lambdas

## Defining Functions

```synthflow
fn greet(name) {
    print("Hello, " + name + "!")
}

greet("Alice")  # Output: Hello, Alice!
greet("Bob")    # Output: Hello, Bob!
```

## Return Values

```synthflow
fn add(a, b) {
    return a + b
}

let sum = add(5, 3)
print(sum)  # Output: 8
```

## Multiple Parameters

```synthflow
fn calculate_area(width, height) {
    return width * height
}

let area = calculate_area(10, 5)
print("Area:", area)  # Output: Area: 50
```

## Default Return

Functions without explicit return statement return `null`:

```synthflow
fn say_hello() {
    print("Hello!")
    # Implicitly returns null
}

let result = say_hello()
print(result)  # Output: null
```

## Recursion

```synthflow
# Fibonacci using recursion
fn fibonacci(n) {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

print("Fibonacci(10):", fibonacci(10))  # Output: 55
```

### Iterative Version (More Efficient)

```synthflow
fn fibonacciIterative(n) {
    if n <= 1 {
        return n
    }
    let a = 0
    let b = 1
    for (let i = 2; i <= n; i++) {
        let temp = a + b
        a = b
        b = temp
    }
    return b
}

print("Fibonacci(20):", fibonacciIterative(20))  # Output: 6765
```

## Lambda Expressions

Lambdas are anonymous functions defined with `=>`:

```synthflow
# Simple lambda
let double = (x) => x * 2
print(double(5))  # Output: 10

# Lambda with no parameters
let greeting = () => "Hello World"
print(greeting())  # Output: Hello World

# Lambda with multiple parameters
let multiply = (a, b) => a * b
print(multiply(4, 5))  # Output: 20
```

## Higher-Order Functions

Functions that take other functions as arguments:

```synthflow
fn apply_twice(f, x) {
    return f(f(x))
}

let increment = (n) => n + 1
print(apply_twice(increment, 5))  # Output: 7
```

## Practical Example: Calculator

```synthflow
fn calculator(a, b, operation) {
    if operation == "add" {
        return a + b
    } else if operation == "subtract" {
        return a - b
    } else if operation == "multiply" {
        return a * b
    } else if operation == "divide" {
        if b != 0 {
            return a / b
        }
        return "Error: Division by zero"
    }
    return "Unknown operation"
}

print(calculator(10, 5, "add"))       # 15
print(calculator(10, 5, "subtract"))  # 5
print(calculator(10, 5, "multiply"))  # 50
print(calculator(10, 5, "divide"))    # 2
```

---

# Chapter 6: Data Structures

## Arrays

### Creating Arrays

```synthflow
# Empty array
let empty = []

# Array with elements
let numbers = [1, 2, 3, 4, 5]
let names = ["Alice", "Bob", "Charlie"]
let mixed = [1, "hello", true, 3.14]
```

### Accessing Elements

```synthflow
let fruits = ["apple", "banana", "cherry"]

print(fruits[0])  # "apple" (first element)
print(fruits[1])  # "banana"
print(fruits[2])  # "cherry" (last element)
```

### Modifying Arrays

```synthflow
let numbers = [10, 20, 30]

# Update element
numbers[1] = 25
print(numbers)  # [10, 25, 30]

# Add element
append(numbers, 40)
print(numbers)  # [10, 25, 30, 40]
```

### Array Length

```synthflow
let items = [1, 2, 3, 4, 5]
print(len(items))  # 5
```

### Iterating Arrays

```synthflow
let colors = ["red", "green", "blue"]

for (let i = 0; i < len(colors); i++) {
    print("Color " + str(i) + ": " + colors[i])
}
```

### Nested Arrays

```synthflow
let matrix = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
]

print(matrix[0][0])  # 1
print(matrix[1][1])  # 5
print(matrix[2][2])  # 9
```

## Maps (Dictionaries)

### Creating Maps

```synthflow
# Map literal syntax
let person = {
    name: "Alice",
    age: 30,
    city: "New York"
}
```

### Accessing Values

```synthflow
print(person.name)  # "Alice"
print(person.age)   # 30
```

### Modifying Maps

```synthflow
# Update value
person.age = 31

# Add new key
person.email = "alice@example.com"
```

### Nested Maps

```synthflow
let company = {
    name: "TechCorp",
    address: {
        street: "123 Main St",
        city: "San Francisco",
        zip: "94102"
    },
    employees: 100
}

print(company.address.city)  # "San Francisco"
```

## Practical Example: Student Records

```synthflow
let students = [
    { name: "Alice", grade: 95, subjects: ["Math", "Science"] },
    { name: "Bob", grade: 87, subjects: ["English", "History"] },
    { name: "Charlie", grade: 92, subjects: ["Math", "Art"] }
]

# Print all students
for (let i = 0; i < len(students); i++) {
    let student = students[i]
    print("Name: " + student.name)
    print("Grade: " + str(student.grade))
    print("---")
}

# Calculate average grade
let total = 0
for (let i = 0; i < len(students); i++) {
    total = total + students[i].grade
}
let average = total / len(students)
print("Class Average: " + str(average))
```

---

# Part II: Core Programming

---

# Chapter 7: Object-Oriented Programming

## Structs

Structs are custom data types that group related data:

```synthflow
# Define a struct (conceptual - using map literals)
fn createPerson(name, age, email) {
    return {
        name: name,
        age: age,
        email: email
    }
}

let alice = createPerson("Alice", 30, "alice@example.com")
print(alice.name)   # "Alice"
print(alice.age)    # 30
```

## Methods

Attach functions to objects:

```synthflow
fn createRectangle(width, height) {
    return {
        width: width,
        height: height,
        area: () => width * height,
        perimeter: () => 2 * (width + height)
    }
}

let rect = createRectangle(10, 5)
print("Area:", rect.area())          # 50
print("Perimeter:", rect.perimeter()) # 30
```

## Factory Pattern

```synthflow
fn createCounter(initial) {
    let count = initial
    
    return {
        increment: () => {
            count = count + 1
            return count
        },
        decrement: () => {
            count = count - 1
            return count
        },
        getValue: () => count,
        reset: () => {
            count = initial
            return count
        }
    }
}

let counter = createCounter(0)
print(counter.increment())  # 1
print(counter.increment())  # 2
print(counter.decrement())  # 1
print(counter.reset())      # 0
```

## Complex Object Example

```synthflow
fn createBankAccount(owner, initialBalance) {
    let balance = initialBalance
    let transactions = []
    
    return {
        owner: owner,
        getBalance: () => balance,
        deposit: (amount) => {
            if amount > 0 {
                balance = balance + amount
                append(transactions, { type: "deposit", amount: amount })
                return true
            }
            return false
        },
        withdraw: (amount) => {
            if amount > 0 && amount <= balance {
                balance = balance - amount
                append(transactions, { type: "withdraw", amount: amount })
                return true
            }
            return false
        },
        getTransactions: () => transactions
    }
}

let account = createBankAccount("Alice", 1000)
account.deposit(500)
account.withdraw(200)
print("Balance:", account.getBalance())  # 1300
```

---

# Chapter 8: Error Handling

## Try/Catch Blocks

SynthFlow supports structured error handling:

```synthflow
try {
    let result = riskyOperation()
    print("Success:", result)
} catch (error) {
    print("Error occurred:", error)
}
```

## Safe Division Example

```synthflow
fn safeDivide(a, b) {
    try {
        if b == 0 {
            throw "Division by zero"
        }
        return a / b
    } catch (e) {
        print("Error:", e)
        return null
    }
}

print(safeDivide(10, 2))   # 5
print(safeDivide(10, 0))   # Error: Division by zero, null
```

## Validation Pattern

```synthflow
fn validateEmail(email) {
    if len(email) == 0 {
        return { valid: false, error: "Email cannot be empty" }
    }
    # Simple check for @ symbol
    let hasAt = false
    for (let i = 0; i < len(email); i++) {
        if email[i] == "@" {
            hasAt = true
            break
        }
    }
    if !hasAt {
        return { valid: false, error: "Email must contain @" }
    }
    return { valid: true, error: null }
}

let result = validateEmail("test@example.com")
if result.valid {
    print("Email is valid!")
} else {
    print("Invalid:", result.error)
}
```

---

# Chapter 9: File I/O & System Operations

## Reading Files

```synthflow
# Read entire file
let content = read_file("data.txt")
print(content)
```

## Writing Files

```synthflow
# Write to file (creates or overwrites)
write_file("output.txt", "Hello, World!")

# Verify
let saved = read_file("output.txt")
print(saved)  # "Hello, World!"
```

## Working with JSON Files

```synthflow
import json

# Read JSON file
let jsonContent = read_file("config.json")
let config = json.parse(jsonContent)
print(config.setting1)

# Write JSON file
let data = { name: "Alice", score: 100 }
let jsonString = json.stringify(data)
write_file("data.json", jsonString)
```

## Practical Example: Log File

```synthflow
fn logMessage(filename, message) {
    let timestamp = "2025-12-14 10:30:00"  # Would use datetime in real code
    let logEntry = "[" + timestamp + "] " + message + "\n"
    
    # Read existing content
    let existing = ""
    try {
        existing = read_file(filename)
    } catch (e) {
        # File doesn't exist, start fresh
    }
    
    # Append new entry
    write_file(filename, existing + logEntry)
}

logMessage("app.log", "Application started")
logMessage("app.log", "User logged in")
logMessage("app.log", "Data processed successfully")
```

---

# Chapter 10: Web Development

## SynthFlow Web Framework

SynthFlow includes a built-in web framework for creating HTTP servers with minimal code.

## Basic Server

```synthflow
# Define routes
route("/", text("Welcome to SynthFlow Web!"))
route("/hello", text("Hello, World!"))

# Start server on port 3000
serve(3000)
```

## JSON API

```synthflow
# Data
let users = [
    { id: 1, name: "Alice", email: "alice@example.com" },
    { id: 2, name: "Bob", email: "bob@example.com" }
]

# Routes
route("/", text("API Server Running"))
route("/api/users", json(users))

# Dynamic route with parameter
route("/api/users/:id", (req) => {
    let id = int(req.params.id) - 1
    if id >= 0 && id < len(users) {
        return json(users[id])
    }
    return json({ error: "User not found" })
})

serve(3000)
```

## HTML Response

```synthflow
route("/html", html("""
<!DOCTYPE html>
<html>
<head>
    <title>SynthFlow App</title>
    <style>
        body { font-family: Arial; padding: 20px; }
        h1 { color: #7c3aed; }
    </style>
</head>
<body>
    <h1>Welcome to SynthFlow!</h1>
    <p>This page was generated by SynthFlow.</p>
</body>
</html>
"""))

serve(3000)
```

## POST Requests

```synthflow
let items = []

route("POST /api/items", (req) => {
    let newItem = { id: len(items) + 1, data: req.body }
    append(items, newItem)
    return json({ success: true, item: newItem })
})

route("/api/items", json(items))

serve(3000)
```

## Complete REST API Example

```synthflow
# In-memory database
let todos = [
    { id: 1, title: "Learn SynthFlow", completed: false },
    { id: 2, title: "Build an app", completed: false }
]

# GET all todos
route("/api/todos", json(todos))

# GET single todo
route("/api/todos/:id", (req) => {
    let id = int(req.params.id)
    for (let i = 0; i < len(todos); i++) {
        if todos[i].id == id {
            return json(todos[i])
        }
    }
    return json({ error: "Todo not found" })
})

# POST new todo
route("POST /api/todos", (req) => {
    let newId = len(todos) + 1
    let todo = { id: newId, title: req.body, completed: false }
    append(todos, todo)
    return json(todo)
})

# Home page
route("/", html("<h1>Todo API</h1><p>Try /api/todos</p>"))

print("Starting Todo API server on port 3000...")
serve(3000)
```

---

# Part III: Cutting-Edge Features

---

# Chapter 11: AI Integration

## Google Gemini API

SynthFlow has **native** integration with Google's Gemini AI:

### Setting Up

```synthflow
# Set your API key (get from https://ai.google.dev)
gemini_set_api_key("YOUR_API_KEY_HERE")

# Check if key is set
if gemini_has_api_key() {
    print("API key configured!")
}
```

### Simple Completion

```synthflow
# Ask Gemini a question
let response = gemini_complete("What is the capital of France?")
print(response)  # "Paris"
```

### Chat with System Instructions

```synthflow
let answer = gemini_chat(
    "You are a helpful math tutor. Be concise.",
    "What is the square root of 144?"
)
print(answer)  # "The square root of 144 is 12."
```

### Specify Model

```synthflow
let response = gemini_chat(
    "You are a coding assistant.",
    "How do I print Hello World in Python?",
    "gemini-2.0-flash"
)
print(response)
```

## Building an AI Chatbot

```synthflow
import io

gemini_set_api_key("YOUR_API_KEY")

let systemPrompt = """
You are a friendly AI assistant called SynthBot.
You help users learn about the SynthFlow programming language.
Keep responses concise and helpful.
"""

print("SynthBot: Hello! I'm SynthBot. Ask me anything about SynthFlow!")
print("Type 'quit' to exit.\n")

while true {
    print("You: ")
    let userInput = input()
    
    if userInput == "quit" {
        print("SynthBot: Goodbye!")
        break
    }
    
    let response = gemini_chat(systemPrompt, userInput)
    print("SynthBot: " + response + "\n")
}
```

## Agent Development (SADK)

The SynthFlow Agent Development Kit enables building intelligent agents:

```synthflow
import agent

# Create an agent
let myAgent = agent.create({
    name: "ResearchAgent",
    description: "An agent that helps with research tasks",
    model: "gemini-2.0-flash"
})

# Define agent tools
agent.addTool(myAgent, {
    name: "search",
    description: "Search for information",
    execute: (query) => {
        # Implement search logic
        return "Search results for: " + query
    }
})

# Run the agent
let result = agent.run(myAgent, "Find information about quantum computing")
print(result)
```

---

# Chapter 12: Quantum Computing

## Introduction to Quantum Computing

SynthFlow includes native support for quantum computing simulation, making it perfect for learning and experimenting with quantum algorithms.

## Complex Numbers

Quantum states use complex numbers. SynthFlow supports complex arithmetic:

```synthflow
# Create complex numbers
fn complex(real, imag) {
    return { real: real, imag: imag }
}

# Complex absolute value squared (probability)
fn complexAbsSquared(c) {
    return c.real * c.real + c.imag * c.imag
}

let z = complex(3, 4)
print("Magnitude squared:", complexAbsSquared(z))  # 25
```

## Qubit States

```synthflow
const SQRT2_INV = 0.70710678118654752440  # 1/√2

# |0⟩ state: amplitude (1, 0) for |0⟩, (0, 0) for |1⟩
let state0_zero = complex(1.0, 0.0)
let state0_one = complex(0.0, 0.0)

print("State |0⟩:")
print("  P(0) = " + str(complexAbsSquared(state0_zero)))  # 1.0
print("  P(1) = " + str(complexAbsSquared(state0_one)))   # 0.0
```

## Superposition

After applying Hadamard gate to |0⟩:

```synthflow
# H|0⟩ = (|0⟩ + |1⟩)/√2
let h0 = complex(SQRT2_INV, 0.0)
let h1 = complex(SQRT2_INV, 0.0)

print("State H|0⟩ = |+⟩:")
print("  P(0) = " + str(complexAbsSquared(h0)))  # 0.5
print("  P(1) = " + str(complexAbsSquared(h1)))  # 0.5
print("Equal superposition achieved!")
```

## Bell State (Entanglement)

```synthflow
# Bell state: (|00⟩ + |11⟩)/√2
# Created by: H on qubit 0, then CNOT(0, 1)

let b00 = complex(SQRT2_INV, 0.0)
let b01 = complex(0.0, 0.0)
let b10 = complex(0.0, 0.0)
let b11 = complex(SQRT2_INV, 0.0)

print("Bell state (|00⟩ + |11⟩)/√2:")
print("  P(00) = " + str(complexAbsSquared(b00)))  # 0.5
print("  P(01) = " + str(complexAbsSquared(b01)))  # 0.0
print("  P(10) = " + str(complexAbsSquared(b10)))  # 0.0
print("  P(11) = " + str(complexAbsSquared(b11)))  # 0.5
print("Qubits are maximally entangled!")
```

## Using the Quantum Library

```synthflow
import quantum

# Create a 2-qubit system initialized to |00⟩
let qs = quantum.createSystem(2)

# Apply Hadamard gate to first qubit -> Superposition
qs.applyGate("H", 0)

# Apply CNOT gate (Control: 0, Target: 1) -> Entanglement
qs.applyCNOT(0, 1)

# Measure the system
let result = qs.measureAll()
print("Measurement result:", result)
# Will output either "00" or "11" with 50% probability each
```

## Quantum Gates Reference

| Gate | Symbol | Matrix | Effect |
|------|--------|--------|--------|
| Hadamard | H | [[1,1],[1,-1]]/√2 | Creates superposition |
| Pauli-X | X | [[0,1],[1,0]] | Bit flip |
| Pauli-Y | Y | [[0,-i],[i,0]] | Bit + phase flip |
| Pauli-Z | Z | [[1,0],[0,-1]] | Phase flip |
| CNOT | CX | 4x4 matrix | Controlled NOT |

---

# Chapter 13: Machine Learning & Data Science

## DataFrame Operations

SynthFlow's DataFrame module provides pandas-like functionality:

```synthflow
import dataframe

# Create a DataFrame
let df = dataframe.create({
    name: ["Alice", "Bob", "Charlie", "Diana"],
    age: [25, 30, 35, 28],
    salary: [50000, 60000, 75000, 55000]
})

# Display info
dataframe.head(df, 2)
dataframe.describe(df)
```

## Basic Statistics

```synthflow
import dataframe

let data = dataframe.create({
    values: [10, 20, 30, 40, 50]
})

print("Mean:", dataframe.mean(data, "values"))      # 30
print("Sum:", dataframe.sum(data, "values"))        # 150
print("Min:", dataframe.min(data, "values"))        # 10
print("Max:", dataframe.max(data, "values"))        # 50
```

## Neural Networks

```synthflow
import neural_network

# Create a simple neural network
let model = neural_network.Sequential([
    neural_network.Dense(64, activation: "relu", input_shape: [10]),
    neural_network.Dense(32, activation: "relu"),
    neural_network.Dense(1, activation: "sigmoid")
])

# Compile the model
neural_network.compile(model, {
    optimizer: "adam",
    loss: "binary_crossentropy",
    metrics: ["accuracy"]
})

# Summary
neural_network.summary(model)
```

## Classification

```synthflow
import classification

# Create classifier
let clf = classification.RandomForest({
    n_estimators: 100,
    max_depth: 10
})

# Train
classification.fit(clf, X_train, y_train)

# Predict
let predictions = classification.predict(clf, X_test)

# Evaluate
let accuracy = classification.accuracy(y_test, predictions)
print("Accuracy:", accuracy)
```

## Data Visualization (ASCII)

```synthflow
import plotting

# Bar chart
plotting.bar({
    data: [10, 25, 15, 30, 20],
    labels: ["A", "B", "C", "D", "E"],
    title: "Sales by Category"
})

# Line chart  
plotting.line({
    data: [1, 4, 2, 5, 3, 6],
    title: "Trend Over Time"
})
```

Output:
```
Sales by Category
A: ██████████ 10
B: █████████████████████████ 25
C: ███████████████ 15
D: ██████████████████████████████ 30
E: ████████████████████ 20
```

---

# Chapter 14: Best Practices & Projects

## Code Style Guide

### Naming Conventions

```synthflow
# Variables: camelCase
let userName = "Alice"
let itemCount = 42

# Constants: UPPER_SNAKE_CASE
const MAX_RETRIES = 3
const API_BASE_URL = "https://api.example.com"

# Functions: camelCase
fn calculateTotal(items) { ... }
fn fetchUserData(userId) { ... }
```

### Code Organization

```synthflow
# 1. Imports at the top
import json
import http

# 2. Constants
const VERSION = "1.0.0"

# 3. Helper functions
fn formatDate(date) { ... }

# 4. Main logic
fn main() {
    # Application code
}

# 5. Entry point
main()
```

## Project 1: Todo List CLI

```synthflow
let todos = []

fn addTodo(title) {
    let todo = {
        id: len(todos) + 1,
        title: title,
        completed: false
    }
    append(todos, todo)
    print("Added: " + title)
}

fn listTodos() {
    print("\n=== Todo List ===")
    for (let i = 0; i < len(todos); i++) {
        let todo = todos[i]
        let status = ""
        if todo.completed {
            status = "[x]"
        } else {
            status = "[ ]"
        }
        print(str(todo.id) + ". " + status + " " + todo.title)
    }
    print("")
}

fn completeTodo(id) {
    for (let i = 0; i < len(todos); i++) {
        if todos[i].id == id {
            todos[i].completed = true
            print("Completed: " + todos[i].title)
            return
        }
    }
    print("Todo not found")
}

# Demo
addTodo("Learn SynthFlow")
addTodo("Build a project")
addTodo("Share with friends")
listTodos()
completeTodo(1)
listTodos()
```

## Project 2: Weather API Client

```synthflow
import http

const API_KEY = "YOUR_WEATHER_API_KEY"

fn getWeather(city) {
    let url = "https://api.weatherapi.com/v1/current.json"
    url = url + "?key=" + API_KEY
    url = url + "&q=" + city
    
    let response = http.get(url)
    let data = http.parseJson(response)
    
    print("\n=== Weather for " + city + " ===")
    print("Temperature: " + str(data.current.temp_c) + "°C")
    print("Condition: " + data.current.condition.text)
    print("Humidity: " + str(data.current.humidity) + "%")
    print("Wind: " + str(data.current.wind_kph) + " km/h")
}

getWeather("London")
getWeather("Tokyo")
```

## Project 3: Number Guessing Game

```synthflow
fn randomNumber(min, max) {
    # Simple pseudo-random for demo
    return min + ((max - min) / 2)  # Would use proper random in real code
}

fn numberGuessingGame() {
    let secret = 42  # In real code: randomNumber(1, 100)
    let attempts = 0
    let maxAttempts = 7
    
    print("=== Number Guessing Game ===")
    print("I'm thinking of a number between 1 and 100.")
    print("You have " + str(maxAttempts) + " attempts.\n")
    
    while attempts < maxAttempts {
        attempts = attempts + 1
        print("Attempt " + str(attempts) + ": Enter your guess:")
        let guess = 42  # In real code: int(input())
        
        if guess == secret {
            print("\n🎉 Congratulations! You guessed it!")
            print("The number was " + str(secret))
            print("You got it in " + str(attempts) + " attempts!")
            return
        } else if guess < secret {
            print("Too low! Try higher.")
        } else {
            print("Too high! Try lower.")
        }
    }
    
    print("\nGame Over! The number was " + str(secret))
}

numberGuessingGame()
```

---

# Appendix A: Quick Reference

## Keywords

| Keyword | Description |
|---------|-------------|
| `let` | Declare mutable variable |
| `const` | Declare constant |
| `fn` | Define function |
| `return` | Return from function |
| `if` | Conditional |
| `else` | Alternative branch |
| `while` | While loop |
| `for` | For loop |
| `break` | Exit loop |
| `continue` | Skip iteration |
| `try` | Try block |
| `catch` | Catch exception |
| `import` | Import module |
| `true` | Boolean true |
| `false` | Boolean false |
| `null` | Null value |

## Operators

| Operator | Description |
|----------|-------------|
| `+`, `-`, `*`, `/`, `%` | Arithmetic |
| `==`, `!=`, `<`, `>`, `<=`, `>=` | Comparison |
| `&&`, `\|\|`, `!` | Logical |
| `=`, `+=`, `-=`, `*=`, `/=` | Assignment |
| `++`, `--` | Increment/Decrement |

## Built-in Functions

| Function | Description |
|----------|-------------|
| `print(...)` | Print to console |
| `len(x)` | Get length |
| `str(x)` | Convert to string |
| `int(x)` | Convert to integer |
| `float(x)` | Convert to float |
| `typeof(x)` | Get type name |
| `append(arr, item)` | Add to array |
| `read_file(path)` | Read file |
| `write_file(path, content)` | Write file |

## Math Functions

| Function | Description |
|----------|-------------|
| `sqrt(x)` | Square root |
| `pow(x, y)` | Power |
| `abs(x)` | Absolute value |
| `sin(x)`, `cos(x)`, `tan(x)` | Trigonometry |
| `floor(x)`, `ceil(x)`, `round(x)` | Rounding |

---

# Appendix B: Standard Library

SynthFlow includes **40+ standard library modules**:

| Module | Description |
|--------|-------------|
| `stdlib/ai.sf` | AI model integration |
| `stdlib/agent.sf` | Agent framework (SADK) |
| `stdlib/array.sf` | Array utilities |
| `stdlib/classification.sf` | ML classifiers |
| `stdlib/clustering.sf` | Clustering algorithms |
| `stdlib/config.sf` | Configuration management |
| `stdlib/database.sf` | Database connectivity |
| `stdlib/dataframe.sf` | Pandas-like data manipulation |
| `stdlib/datetime.sf` | Date/time utilities |
| `stdlib/http.sf` | HTTP client |
| `stdlib/io.sf` | File I/O |
| `stdlib/json.sf` | JSON utilities |
| `stdlib/list.sf` | Linked list |
| `stdlib/logging.sf` | Logging framework |
| `stdlib/map.sf` | Map/dictionary |
| `stdlib/math.sf` | Math functions |
| `stdlib/metrics.sf` | ML metrics |
| `stdlib/mobile.sf` | Mobile dev components |
| `stdlib/neural_network.sf` | Deep learning |
| `stdlib/pipeline.sf` | ML pipelines |
| `stdlib/plotting.sf` | ASCII visualization |
| `stdlib/preprocessing.sf` | Data preprocessing |
| `stdlib/pwa.sf` | PWA helpers |
| `stdlib/quantum.sf` | Quantum computing |
| `stdlib/regression.sf` | Regression models |
| `stdlib/scraping.sf` | Web scraping |
| `stdlib/set.sf` | Set data structure |
| `stdlib/string.sf` | String utilities |
| `stdlib/sysadmin.sf` | System administration |
| `stdlib/testing.sf` | Unit testing |
| `stdlib/vectorstore.sf` | Vector store (RAG) |
| `stdlib/web.sf` | Web framework helpers |

---

# Appendix C: Resources

## Official Resources

- **GitHub Repository**: [github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow](https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow)
- **Documentation**: Available in the `/docs` folder of the repository
- **Examples**: Check `/examples` folder for working code samples

## Getting Help

1. **GitHub Issues**: Report bugs or request features
2. **Discussions**: Ask questions and share ideas
3. **Contributing**: See `CONTRIBUTING.md` for guidelines

## IDE Support

- **VS Code Extension**: Search "SynthFlow" in the marketplace
- **LSP Server**: Full language server protocol support
- **MCP Integration**: Connect to AI IDEs like Claude Desktop

## Version History

| Version | Date | Highlights |
|---------|------|------------|
| 0.0.30 | Dec 2025 | Web framework, Mobile dev, Python parity |
| 0.0.25 | Dec 2025 | Quantum computing, MCP server |
| 0.0.17 | Dec 2025 | Type system, LSP, modules |
| 0.0.3 | Dec 2024 | Const, type annotations, try/catch |
| 0.0.1 | Dec 2024 | Initial release |

---

<div align="center">

## Thank You for Learning SynthFlow!

**You've completed the Zero to Hero journey!**

Now go build something amazing! 🚀

---

*SynthFlow Programming Language*  
*© 2025 WEE Technology Solution*  
*MIT License*

</div>
]]>
