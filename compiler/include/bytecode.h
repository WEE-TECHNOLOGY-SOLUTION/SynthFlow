#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <memory>

// Bytecode instruction set for SynthFlow VM
enum class OpCode : uint8_t {
    // Stack operations
    PUSH_INT,       // Push integer constant
    PUSH_FLOAT,     // Push float constant
    PUSH_STRING,    // Push string constant
    PUSH_BOOL,      // Push boolean constant
    PUSH_NULL,      // Push null value
    POP,            // Pop top of stack
    DUP,            // Duplicate top of stack
    
    // Variable operations
    LOAD_VAR,       // Load variable by index
    STORE_VAR,      // Store variable by index
    LOAD_GLOBAL,    // Load global variable
    STORE_GLOBAL,   // Store global variable
    
    // Arithmetic operations
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,            // Unary negation
    
    // Comparison operations
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    
    // Logical operations
    AND,
    OR,
    NOT,
    
    // Control flow
    JUMP,           // Unconditional jump
    JUMP_IF_FALSE,  // Jump if top of stack is false
    JUMP_IF_TRUE,   // Jump if top of stack is true
    
    // Function operations
    CALL,           // Call function
    RETURN,         // Return from function
    
    // Array operations
    MAKE_ARRAY,     // Create array from N stack elements
    INDEX,          // Array index access
    INDEX_SET,      // Array index assignment
    
    // Built-in operations
    PRINT,          // Print to stdout
    
    // Special
    HALT            // Stop execution
};

// Single bytecode instruction
struct Instruction {
    OpCode opcode;
    uint32_t operand = 0;  // Optional operand (index, jump offset, etc.)
    
    Instruction(OpCode op) : opcode(op), operand(0) {}
    Instruction(OpCode op, uint32_t arg) : opcode(op), operand(arg) {}
};

// Constant pool value types
using ConstantValue = std::variant<
    int64_t,        // Integer
    double,         // Float
    std::string,    // String
    bool            // Boolean
>;

// Compiled function
struct CompiledFunction {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<Instruction> code;
    int localCount = 0;
};

// Bytecode chunk - compiled program
class BytecodeChunk {
public:
    std::vector<Instruction> code;
    std::vector<ConstantValue> constants;
    std::vector<CompiledFunction> functions;
    
    // Add constant to pool, return index
    uint32_t addConstant(const ConstantValue& value) {
        constants.push_back(value);
        return static_cast<uint32_t>(constants.size() - 1);
    }
    
    // Add instruction
    size_t emit(OpCode op) {
        code.push_back(Instruction(op));
        return code.size() - 1;
    }
    
    size_t emit(OpCode op, uint32_t operand) {
        code.push_back(Instruction(op, operand));
        return code.size() - 1;
    }
    
    // Patch jump instruction
    void patchJump(size_t offset, uint32_t target) {
        code[offset].operand = target;
    }
};
