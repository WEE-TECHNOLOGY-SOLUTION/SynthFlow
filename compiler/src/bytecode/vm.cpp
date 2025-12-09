#include "../../include/vm.h"
#include <stdexcept>
#include <sstream>

std::string VMValue::toString() const {
    if (isNull()) return "null";
    if (isInt()) return std::to_string(asInt());
    if (isFloat()) return std::to_string(asFloat());
    if (isString()) return asString();
    if (isBool()) return asBool() ? "true" : "false";
    if (isArray()) {
        std::string result = "[";
        auto arr = std::get<std::shared_ptr<ArrayType>>(data);
        for (size_t i = 0; i < arr->size(); ++i) {
            if (i > 0) result += ", ";
            result += (*arr)[i].toString();
        }
        return result + "]";
    }
    return "<unknown>";
}

bool VMValue::isTruthy() const {
    if (isNull()) return false;
    if (isBool()) return asBool();
    if (isInt()) return asInt() != 0;
    if (isFloat()) return asFloat() != 0.0;
    if (isString()) return !asString().empty();
    if (isArray()) return !std::get<std::shared_ptr<ArrayType>>(data)->empty();
    return true;
}

VM::VM() {
    // Register built-in functions
    registerBuiltin("print", [](std::vector<VMValue>& args) -> VMValue {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << args[i].toString();
        }
        std::cout << "\n";
        return VMValue();
    });
    
    registerBuiltin("len", [](std::vector<VMValue>& args) -> VMValue {
        if (args.empty()) return VMValue(0LL);
        if (args[0].isString()) return VMValue(static_cast<int64_t>(args[0].asString().length()));
        if (args[0].isArray()) return VMValue(static_cast<int64_t>(std::get<std::shared_ptr<VMValue::ArrayType>>(args[0].data)->size()));
        return VMValue(0LL);
    });
    
    registerBuiltin("str", [](std::vector<VMValue>& args) -> VMValue {
        if (args.empty()) return VMValue("");
        return VMValue(args[0].toString());
    });
}

void VM::push(const VMValue& value) {
    stack.push_back(value);
}

VMValue VM::pop() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    VMValue value = stack.back();
    stack.pop_back();
    return value;
}

VMValue& VM::peek(int offset) {
    return stack[stack.size() - 1 - offset];
}

void VM::registerBuiltin(const std::string& name, 
                        std::function<VMValue(std::vector<VMValue>&)> func) {
    builtins[name] = func;
}

bool VM::executeInstruction() {
    if (ip >= chunk->code.size()) return false;
    
    Instruction& instr = chunk->code[ip++];
    
    switch (instr.opcode) {
        case OpCode::PUSH_INT:
            push(VMValue(std::get<int64_t>(chunk->constants[instr.operand])));
            break;
            
        case OpCode::PUSH_FLOAT:
            push(VMValue(std::get<double>(chunk->constants[instr.operand])));
            break;
            
        case OpCode::PUSH_STRING:
            push(VMValue(std::get<std::string>(chunk->constants[instr.operand])));
            break;
            
        case OpCode::PUSH_BOOL:
            push(VMValue(std::get<bool>(chunk->constants[instr.operand])));
            break;
            
        case OpCode::PUSH_NULL:
            push(VMValue());
            break;
            
        case OpCode::POP:
            pop();
            break;
            
        case OpCode::DUP:
            push(peek());
            break;
            
        case OpCode::LOAD_GLOBAL: {
            uint32_t index = instr.operand;
            if (index >= globals.size()) {
                globals.resize(index + 1);
            }
            push(globals[index]);
            break;
        }
        
        case OpCode::STORE_GLOBAL: {
            uint32_t index = instr.operand;
            if (index >= globals.size()) {
                globals.resize(index + 1);
            }
            globals[index] = peek();
            break;
        }
        
        case OpCode::ADD: {
            VMValue right = pop();
            VMValue left = pop();
            if (left.isInt() && right.isInt()) {
                push(VMValue(left.asInt() + right.asInt()));
            } else if (left.isString() || right.isString()) {
                push(VMValue(left.toString() + right.toString()));
            } else {
                push(VMValue(left.asFloat() + right.asFloat()));
            }
            break;
        }
        
        case OpCode::SUB: {
            VMValue right = pop();
            VMValue left = pop();
            if (left.isInt() && right.isInt()) {
                push(VMValue(left.asInt() - right.asInt()));
            } else {
                push(VMValue(left.asFloat() - right.asFloat()));
            }
            break;
        }
        
        case OpCode::MUL: {
            VMValue right = pop();
            VMValue left = pop();
            if (left.isInt() && right.isInt()) {
                push(VMValue(left.asInt() * right.asInt()));
            } else {
                push(VMValue(left.asFloat() * right.asFloat()));
            }
            break;
        }
        
        case OpCode::DIV: {
            VMValue right = pop();
            VMValue left = pop();
            if (right.isInt() && right.asInt() == 0) {
                throw std::runtime_error("Division by zero");
            }
            if (right.isFloat() && right.asFloat() == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            if (left.isInt() && right.isInt()) {
                push(VMValue(left.asInt() / right.asInt()));
            } else {
                push(VMValue(left.asFloat() / right.asFloat()));
            }
            break;
        }
        
        case OpCode::MOD: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.asInt() % right.asInt()));
            break;
        }
        
        case OpCode::NEG:
            if (peek().isInt()) {
                push(VMValue(-pop().asInt()));
            } else {
                push(VMValue(-pop().asFloat()));
            }
            break;
        
        case OpCode::EQ: {
            VMValue right = pop();
            VMValue left = pop();
            if (left.isInt() && right.isInt()) {
                push(VMValue(left.asInt() == right.asInt()));
            } else if (left.isString() && right.isString()) {
                push(VMValue(left.asString() == right.asString()));
            } else {
                push(VMValue(left.asFloat() == right.asFloat()));
            }
            break;
        }
        
        case OpCode::NE: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.toString() != right.toString()));
            break;
        }
        
        case OpCode::LT: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.asFloat() < right.asFloat()));
            break;
        }
        
        case OpCode::GT: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.asFloat() > right.asFloat()));
            break;
        }
        
        case OpCode::LE: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.asFloat() <= right.asFloat()));
            break;
        }
        
        case OpCode::GE: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.asFloat() >= right.asFloat()));
            break;
        }
        
        case OpCode::AND: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.isTruthy() && right.isTruthy()));
            break;
        }
        
        case OpCode::OR: {
            VMValue right = pop();
            VMValue left = pop();
            push(VMValue(left.isTruthy() || right.isTruthy()));
            break;
        }
        
        case OpCode::NOT:
            push(VMValue(!pop().isTruthy()));
            break;
        
        case OpCode::JUMP:
            ip = instr.operand;
            break;
            
        case OpCode::JUMP_IF_FALSE:
            if (!pop().isTruthy()) {
                ip = instr.operand;
            }
            break;
            
        case OpCode::JUMP_IF_TRUE:
            if (pop().isTruthy()) {
                ip = instr.operand;
            }
            break;
        
        case OpCode::PRINT: {
            VMValue val = pop();
            std::cout << val.toString() << "\n";
            break;
        }
        
        case OpCode::MAKE_ARRAY: {
            auto arr = std::make_shared<VMValue::ArrayType>();
            for (uint32_t i = 0; i < instr.operand; ++i) {
                arr->insert(arr->begin(), pop());
            }
            push(VMValue(arr));
            break;
        }
        
        case OpCode::INDEX: {
            VMValue index = pop();
            VMValue arr = pop();
            if (!arr.isArray()) {
                throw std::runtime_error("Cannot index non-array");
            }
            auto arrPtr = std::get<std::shared_ptr<VMValue::ArrayType>>(arr.data);
            size_t idx = static_cast<size_t>(index.asInt());
            if (idx >= arrPtr->size()) {
                throw std::runtime_error("Array index out of bounds");
            }
            push((*arrPtr)[idx]);
            break;
        }
        
        case OpCode::HALT:
            return false;
            
        default:
            throw std::runtime_error("Unknown opcode");
    }
    
    return true;
}

void VM::run(BytecodeChunk& bytecode) {
    chunk = &bytecode;
    ip = 0;
    stack.clear();
    globals.clear();
    
    while (executeInstruction()) {
        // Continue execution
    }
}
