#pragma once
#include "bytecode.h"
#include <vector>
#include <stack>
#include <memory>
#include <variant>
#include <functional>
#include <string>
#include <unordered_map>
#include <iostream>

// Runtime value for VM
class VMValue {
public:
    using ArrayType = std::vector<VMValue>;
    
    std::variant<
        std::monostate,  // null
        int64_t,         // integer
        double,          // float
        std::string,     // string
        bool,            // boolean
        std::shared_ptr<ArrayType>  // array
    > data;
    
    VMValue() : data(std::monostate{}) {}
    VMValue(int64_t v) : data(v) {}
    VMValue(double v) : data(v) {}
    VMValue(const std::string& v) : data(v) {}
    VMValue(bool v) : data(v) {}
    VMValue(std::shared_ptr<ArrayType> v) : data(v) {}
    
    bool isNull() const { return std::holds_alternative<std::monostate>(data); }
    bool isInt() const { return std::holds_alternative<int64_t>(data); }
    bool isFloat() const { return std::holds_alternative<double>(data); }
    bool isString() const { return std::holds_alternative<std::string>(data); }
    bool isBool() const { return std::holds_alternative<bool>(data); }
    bool isArray() const { return std::holds_alternative<std::shared_ptr<ArrayType>>(data); }
    
    int64_t asInt() const { return std::get<int64_t>(data); }
    double asFloat() const { 
        if (isInt()) return static_cast<double>(asInt());
        return std::get<double>(data); 
    }
    const std::string& asString() const { return std::get<std::string>(data); }
    bool asBool() const { return std::get<bool>(data); }
    
    std::string toString() const;
    bool isTruthy() const;
};

// Virtual Machine - executes bytecode
class VM {
private:
    BytecodeChunk* chunk;
    size_t ip = 0;  // Instruction pointer
    std::vector<VMValue> stack;
    std::vector<VMValue> globals;
    
    // Call frame for function calls
    struct CallFrame {
        size_t returnAddress;
        size_t stackBase;
    };
    std::stack<CallFrame> callStack;
    
    // Built-in functions
    std::unordered_map<std::string, std::function<VMValue(std::vector<VMValue>&)>> builtins;
    
    // Stack operations
    void push(const VMValue& value);
    VMValue pop();
    VMValue& peek(int offset = 0);
    
    // Execute single instruction
    bool executeInstruction();
    
public:
    VM();
    
    // Run bytecode
    void run(BytecodeChunk& bytecode);
    
    // Register built-in function
    void registerBuiltin(const std::string& name, 
                        std::function<VMValue(std::vector<VMValue>&)> func);
};
