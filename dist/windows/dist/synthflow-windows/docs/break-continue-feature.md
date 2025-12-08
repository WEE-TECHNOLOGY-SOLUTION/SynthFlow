# Break and Continue Statement Implementation

This document describes the implementation of break and continue statements in the SynthFlow programming language.

## Feature Overview

Break and continue statements are control flow statements that allow for more sophisticated loop control:

- **break**: Immediately exits the innermost enclosing loop
- **continue**: Skips the rest of the current iteration and continues with the next iteration

## Syntax

```
while (condition) {
    // statements
    break;    // exits the loop
    // statements
    continue; // skips to next iteration
    // statements
}

for (initialization; condition; increment) {
    // statements
    break;    // exits the loop
    // statements
    continue; // skips to next iteration
    // statements
}
```

## Implementation Details

### 1. Token Definitions

The break and continue keywords were already defined in the [token.h](file:///D:/SynthFlow/synthflow/compiler/include/token.h) file as `KW_BREAK` and `KW_CONTINUE`.

### 2. AST Nodes

New AST nodes were added to represent break and continue statements:

```cpp
class BreakStatement : public Statement {
public:
    void accept(ASTVisitor* visitor) override;
};

class ContinueStatement : public Statement {
public:
    void accept(ASTVisitor* visitor) override;
};
```

### 3. Parser Enhancement

The parser was enhanced to recognize and parse break and continue statements:

```cpp
std::unique_ptr<Statement> Parser::parseBreakStatement() {
    advance(); // consume 'break'
    match(TokenType::SEMICOLON); // Optional semicolon
    return std::make_unique<BreakStatement>();
}

std::unique_ptr<Statement> Parser::parseContinueStatement() {
    advance(); // consume 'continue'
    match(TokenType::SEMICOLON); // Optional semicolon
    return std::make_unique<ContinueStatement>();
}
```

### 4. Semantic Analysis

The semantic analyzer was enhanced to validate that break and continue statements are only used inside loops:

```cpp
void SemanticAnalyzer::visit(BreakStatement* node) {
    // Check that we're inside a loop
    if (loopContext.empty()) {
        reportError("break statement not allowed outside of loop");
    }
}

void SemanticAnalyzer::visit(ContinueStatement* node) {
    // Check that we're inside a loop
    if (loopContext.empty()) {
        reportError("continue statement not allowed outside of loop");
    }
}
```

A stack-based approach is used to track loop context:
- When entering a loop (while or for), `true` is pushed onto the stack
- When exiting a loop, the value is popped from the stack
- When encountering a break or continue statement, the analyzer checks if the stack is empty

### 5. Code Generation

The code generator was enhanced to produce appropriate output for break and continue statements:

```cpp
void CodeGenerator::visit(BreakStatement* node) {
    output += "  break;\n";
}

void CodeGenerator::visit(ContinueStatement* node) {
    output += "  continue;\n";
}
```

## Testing

Tests were created to verify the correct behavior of break and continue statements:

1. **Valid usage**: Break and continue statements inside loops should be accepted
2. **Invalid usage**: Break and continue statements outside loops should be rejected with appropriate error messages
3. **Integration**: Full compilation pipeline should work correctly with break and continue statements

## Example Usage

```synthflow
// While loop with break and continue
let counter = 0;
while (counter < 10) {
    counter = counter + 1;
    if (counter == 3) {
        continue; // Skip the rest of the loop body when counter is 3
    }
    if (counter == 7) {
        break; // Exit the loop when counter is 7
    }
    let temp = counter * 2;
}

// For loop with break and continue
for (let i = 0; i < 5; i = i + 1) {
    if (i == 2) {
        continue; // Skip the rest of the loop body when i is 2
    }
    if (i == 4) {
        break; // Exit the loop when i is 4
    }
    let square = i * i;
}
```