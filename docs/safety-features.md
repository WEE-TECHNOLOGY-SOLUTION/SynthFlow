# SynthFlow Safety Features Guide

SynthFlow v0.0.3 introduces comprehensive safety features to help you write more reliable code.

## 1. Immutable Variables (`const`)

Use `const` to declare variables that cannot be reassigned.

```synthflow
const PI = 3.14159
const MAX_RETRIES = 3
const APP_NAME = "MyApp"

// Error: Cannot reassign constant 'PI'
PI = 3.0  // ❌ This will fail!
```

### When to Use

- Configuration values
- Mathematical constants
- Values that should never change

---

## 2. Type Annotations

Add optional type hints for better documentation and future type checking.

```synthflow
// Basic types
let count: int = 42
let price: float = 19.99
let name: string = "Alice"
let active: bool = true

// Type annotations are optional
let x = 10          // Works fine
let y: int = 10     // Also works, more explicit
```

### Available Types

| Type | Description | Example |
|------|-------------|---------|
| `int` | Integer numbers | `let x: int = 42` |
| `float` | Floating-point | `let y: float = 3.14` |
| `string` | Text | `let s: string = "hi"` |
| `bool` | Boolean | `let b: bool = true` |

---

## 3. Null Safety

SynthFlow supports null values and nullable types.

```synthflow
// null literal
let empty = null

// Nullable types with ?
let maybeValue: int? = null
let optionalName: string? = null

// Can later assign a value
maybeValue = 42
optionalName = "Bob"
```

### Best Practices

```synthflow
// Check for null before using
if (maybeValue != null) {
    print("Value:", maybeValue)
}
```

---

## 4. Error Handling (`try/catch`)

Handle runtime errors gracefully with try/catch blocks.

```synthflow
try {
    let result = 10 / 0  // Division by zero
    print("Result:", result)
} catch (error) {
    print("Error occurred:", error)
}

print("Program continues safely!")
```

### Common Use Cases

```synthflow
// File operations
try {
    let content = read_file("config.txt")
    print(content)
} catch (error) {
    print("Could not read file:", error)
}

// User input parsing
try {
    let num = int(input("Enter a number: "))
    print("You entered:", num)
} catch (error) {
    print("Invalid number")
}
```

---

## Complete Example

```synthflow
// Configuration constants
const MAX_ATTEMPTS = 3
const TIMEOUT: int = 5000

// Nullable user data
let username: string? = null
let loginAttempts: int = 0

fn authenticate(user, password) {
    try {
        // Simulated authentication
        if (user == "admin" && password == "secret") {
            return true
        }
        return false
    } catch (error) {
        print("Auth error:", error)
        return false
    }
}

// Main logic
while (loginAttempts < MAX_ATTEMPTS) {
    let user = input("Username: ")
    let pass = input("Password: ")
    
    if (authenticate(user, pass)) {
        username = user
        print("Welcome,", username)
        break
    }
    
    loginAttempts = loginAttempts + 1
    print("Invalid credentials. Attempts:", loginAttempts)
}

if (username == null) {
    print("Too many failed attempts")
}
```

---

## Summary

| Feature | Purpose | Example |
|---------|---------|---------|
| `const` | Immutable variables | `const PI = 3.14` |
| Type annotations | Type hints | `let x: int = 10` |
| Null safety | Nullable types | `let x: int? = null` |
| try/catch | Error handling | `try { } catch (e) { }` |
