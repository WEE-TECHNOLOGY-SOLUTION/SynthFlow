#include "../../include/interpreter.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <fstream>

// Value methods
std::string Value::toString() const {
    if (isNull()) return "null";
    if (isInt()) return std::to_string(asInt());
    if (isFloat()) {
        std::ostringstream oss;
        oss << asFloat();
        return oss.str();
    }
    if (isString()) return asString();
    if (isBool()) return asBool() ? "true" : "false";
    if (isArray()) {
        std::string result = "[";
        auto arr = asArray();
        for (size_t i = 0; i < arr->size(); ++i) {
            if (i > 0) result += ", ";
            result += (*arr)[i].toString();
        }
        result += "]";
        return result;
    }
    if (isFunction()) return "<function>";
    return "<unknown>";
}

bool Value::isTruthy() const {
    if (isNull()) return false;
    if (isBool()) return asBool();
    if (isInt()) return asInt() != 0;
    if (isFloat()) return asFloat() != 0.0;
    if (isString()) return !asString().empty();
    if (isArray()) return !asArray()->empty();
    return true;
}

// Environment methods
void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
}

Value Environment::get(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    if (parent) {
        return parent->get(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

void Environment::set(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    if (parent) {
        parent->set(name, value);
        return;
    }
    throw std::runtime_error("Undefined variable: " + name);
}

bool Environment::exists(const std::string& name) const {
    if (variables.find(name) != variables.end()) return true;
    if (parent) return parent->exists(name);
    return false;
}

// Interpreter constructor
Interpreter::Interpreter() {
    globalEnv = std::make_shared<Environment>();
    currentEnv = globalEnv;
    registerBuiltins();
}

// Register built-in functions
void Interpreter::registerBuiltins() {
    // print function
    globalEnv->define("print", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            for (size_t i = 0; i < args.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << args[i].toString();
            }
            std::cout << std::endl;
            return Value();
        }
    )));
    
    // input function
    globalEnv->define("input", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (!args.empty()) {
                std::cout << args[0].toString();
            }
            std::string line;
            std::getline(std::cin, line);
            return Value(line);
        }
    )));
    
    // len function
    globalEnv->define("len", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("len() requires an argument");
            if (args[0].isString()) {
                return Value(static_cast<int64_t>(args[0].asString().length()));
            }
            if (args[0].isArray()) {
                return Value(static_cast<int64_t>(args[0].asArray()->size()));
            }
            throw std::runtime_error("len() requires a string or array");
        }
    )));
    
    // str function
    globalEnv->define("str", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value("");
            return Value(args[0].toString());
        }
    )));
    
    // int function
    globalEnv->define("int", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value(int64_t(0));
            if (args[0].isInt()) return args[0];
            if (args[0].isFloat()) return Value(static_cast<int64_t>(args[0].asFloat()));
            if (args[0].isString()) {
                try {
                    return Value(std::stoll(args[0].asString()));
                } catch (...) {
                    throw std::runtime_error("Cannot convert string to int");
                }
            }
            if (args[0].isBool()) return Value(args[0].asBool() ? int64_t(1) : int64_t(0));
            throw std::runtime_error("Cannot convert to int");
        }
    )));
    
    // float function
    globalEnv->define("float", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value(0.0);
            if (args[0].isFloat()) return args[0];
            if (args[0].isInt()) return Value(static_cast<double>(args[0].asInt()));
            if (args[0].isString()) {
                try {
                    return Value(std::stod(args[0].asString()));
                } catch (...) {
                    throw std::runtime_error("Cannot convert string to float");
                }
            }
            throw std::runtime_error("Cannot convert to float");
        }
    )));
    
    // read_file function
    globalEnv->define("read_file", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("read_file() requires a string path");
            }
            std::ifstream file(args[0].asString());
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file: " + args[0].asString());
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return Value(buffer.str());
        }
    )));
    
    // write_file function
    globalEnv->define("write_file", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString()) {
                throw std::runtime_error("write_file() requires path and content");
            }
            std::ofstream file(args[0].asString());
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + args[0].asString());
            }
            file << args[1].toString();
            return Value(true);
        }
    )));
}

// Execute statements
void Interpreter::execute(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
}

// Helper to evaluate expression
Value Interpreter::evaluate(Expression* expr) {
    expr->accept(*this);
    return lastValue;
}

// Call a function
Value Interpreter::callFunction(const std::string& name, std::vector<Value>& args) {
    // Check for user-defined function
    auto it = userFunctions.find(name);
    if (it != userFunctions.end()) {
        const UserFunction& func = it->second;
        
        // Create new environment with closure
        auto funcEnv = std::make_shared<Environment>(func.closure);
        
        // Bind parameters
        for (size_t i = 0; i < func.parameters.size(); ++i) {
            if (i < args.size()) {
                funcEnv->define(func.parameters[i], args[i]);
            } else {
                funcEnv->define(func.parameters[i], Value());
            }
        }
        
        // Save current environment and switch
        auto prevEnv = currentEnv;
        currentEnv = funcEnv;
        
        Value result;
        try {
            func.body->accept(*this);
        } catch (const ReturnException& ret) {
            // Handle return value
            if (std::holds_alternative<int64_t>(ret.value)) {
                result = Value(std::get<int64_t>(ret.value));
            } else if (std::holds_alternative<double>(ret.value)) {
                result = Value(std::get<double>(ret.value));
            } else if (std::holds_alternative<std::string>(ret.value)) {
                result = Value(std::get<std::string>(ret.value));
            } else if (std::holds_alternative<bool>(ret.value)) {
                result = Value(std::get<bool>(ret.value));
            }
        }
        
        // Restore environment
        currentEnv = prevEnv;
        return result;
    }
    
    // Check for built-in function
    if (globalEnv->exists(name)) {
        Value funcVal = globalEnv->get(name);
        if (funcVal.isFunction()) {
            return (*funcVal.asFunction())(args, *this);
        }
    }
    
    throw std::runtime_error("Undefined function: " + name);
}

// Visitor implementations
void Interpreter::visit(IntegerLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(FloatLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(StringLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(BooleanLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(NullLiteral* node) {
    lastValue = Value();  // monostate = null
}

void Interpreter::visit(Identifier* node) {
    lastValue = currentEnv->get(node->name);
}

void Interpreter::visit(BinaryExpression* node) {
    Value left = evaluate(node->left.get());
    Value right = evaluate(node->right.get());
    const std::string& op = node->op;
    
    // Arithmetic operations
    if (op == "+") {
        if (left.isString() || right.isString()) {
            lastValue = Value(left.toString() + right.toString());
        } else if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() + right.asFloat());
        } else {
            lastValue = Value(left.asInt() + right.asInt());
        }
    } else if (op == "-") {
        if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() - right.asFloat());
        } else {
            lastValue = Value(left.asInt() - right.asInt());
        }
    } else if (op == "*") {
        if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() * right.asFloat());
        } else {
            lastValue = Value(left.asInt() * right.asInt());
        }
    } else if (op == "/") {
        if (right.asFloat() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        lastValue = Value(left.asFloat() / right.asFloat());
    } else if (op == "%") {
        lastValue = Value(left.asInt() % right.asInt());
    }
    // Comparison operations
    else if (op == "==") {
        if (left.isString() && right.isString()) {
            lastValue = Value(left.asString() == right.asString());
        } else if (left.isNumber() && right.isNumber()) {
            lastValue = Value(left.asFloat() == right.asFloat());
        } else if (left.isBool() && right.isBool()) {
            lastValue = Value(left.asBool() == right.asBool());
        } else {
            lastValue = Value(false);
        }
    } else if (op == "!=") {
        if (left.isString() && right.isString()) {
            lastValue = Value(left.asString() != right.asString());
        } else if (left.isNumber() && right.isNumber()) {
            lastValue = Value(left.asFloat() != right.asFloat());
        } else {
            lastValue = Value(true);
        }
    } else if (op == "<") {
        lastValue = Value(left.asFloat() < right.asFloat());
    } else if (op == ">") {
        lastValue = Value(left.asFloat() > right.asFloat());
    } else if (op == "<=") {
        lastValue = Value(left.asFloat() <= right.asFloat());
    } else if (op == ">=") {
        lastValue = Value(left.asFloat() >= right.asFloat());
    }
    // Logical operations
    else if (op == "&&" || op == "and") {
        lastValue = Value(left.isTruthy() && right.isTruthy());
    } else if (op == "||" || op == "or") {
        lastValue = Value(left.isTruthy() || right.isTruthy());
    }
    else {
        throw std::runtime_error("Unknown binary operator: " + op);
    }
}

void Interpreter::visit(UnaryExpression* node) {
    Value operand = evaluate(node->operand.get());
    const std::string& op = node->op;
    
    if (op == "-") {
        if (operand.isFloat()) {
            lastValue = Value(-operand.asFloat());
        } else {
            lastValue = Value(-operand.asInt());
        }
    } else if (op == "!" || op == "not") {
        lastValue = Value(!operand.isTruthy());
    } else {
        throw std::runtime_error("Unknown unary operator: " + op);
    }
}

void Interpreter::visit(AssignmentExpression* node) {
    Value value = evaluate(node->right.get());
    
    if (auto* id = dynamic_cast<Identifier*>(node->left.get())) {
        currentEnv->set(id->name, value);
        lastValue = value;
    } else {
        throw std::runtime_error("Invalid assignment target");
    }
}

void Interpreter::visit(CallExpression* node) {
    std::vector<Value> args;
    for (auto& arg : node->arguments) {
        args.push_back(evaluate(arg.get()));
    }
    lastValue = callFunction(node->callee, args);
}

void Interpreter::visit(ArrayLiteral* node) {
    auto arr = std::make_shared<Value::ArrayType>();
    for (auto& elem : node->elements) {
        arr->push_back(evaluate(elem.get()));
    }
    lastValue = Value(arr);
}

void Interpreter::visit(ArrayIndexExpression* node) {
    Value arr = evaluate(node->array.get());
    Value idx = evaluate(node->index.get());
    
    if (!arr.isArray()) {
        throw std::runtime_error("Cannot index non-array");
    }
    if (!idx.isInt()) {
        throw std::runtime_error("Array index must be integer");
    }
    
    size_t index = static_cast<size_t>(idx.asInt());
    if (index >= arr.asArray()->size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    lastValue = (*arr.asArray())[index];
}

void Interpreter::visit(ArrayAssignmentExpression* node) {
    Value arr = evaluate(node->array.get());
    Value idx = evaluate(node->index.get());
    Value val = evaluate(node->value.get());
    
    if (!arr.isArray()) {
        throw std::runtime_error("Cannot index non-array");
    }
    if (!idx.isInt()) {
        throw std::runtime_error("Array index must be integer");
    }
    
    size_t index = static_cast<size_t>(idx.asInt());
    if (index >= arr.asArray()->size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    (*arr.asArray())[index] = val;
    lastValue = val;
}

void Interpreter::visit(VariableDeclaration* node) {
    Value value;
    if (node->initializer) {
        value = evaluate(node->initializer.get());
    }
    currentEnv->define(node->name, value);
    
    // Track if this is a const variable
    if (node->isConst) {
        constVariables[node->name] = true;
    }
}

void Interpreter::visit(ExpressionStatement* node) {
    evaluate(node->expression.get());
}

void Interpreter::visit(BlockStatement* node) {
    auto blockEnv = std::make_shared<Environment>(currentEnv);
    auto prevEnv = currentEnv;
    currentEnv = blockEnv;
    
    for (auto& stmt : node->statements) {
        stmt->accept(*this);
    }
    
    currentEnv = prevEnv;
}

void Interpreter::visit(IfStatement* node) {
    Value condition = evaluate(node->condition.get());
    
    if (condition.isTruthy()) {
        node->thenBranch->accept(*this);
    } else if (node->elseBranch) {
        node->elseBranch->accept(*this);
    }
}

void Interpreter::visit(WhileStatement* node) {
    while (evaluate(node->condition.get()).isTruthy()) {
        try {
            node->body->accept(*this);
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            continue;
        }
    }
}

void Interpreter::visit(ForStatement* node) {
    auto forEnv = std::make_shared<Environment>(currentEnv);
    auto prevEnv = currentEnv;
    currentEnv = forEnv;
    
    if (node->initializer) {
        node->initializer->accept(*this);
    }
    
    while (!node->condition || evaluate(node->condition.get()).isTruthy()) {
        try {
            node->body->accept(*this);
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            // Continue to increment
        }
        
        if (node->increment) {
            evaluate(node->increment.get());
        }
    }
    
    currentEnv = prevEnv;
}

void Interpreter::visit(BreakStatement*) {
    throw BreakException();
}

void Interpreter::visit(ContinueStatement*) {
    throw ContinueException();
}

void Interpreter::visit(FunctionDeclaration* node) {
    UserFunction func;
    func.parameters = node->parameters;
    func.body = node->body.get();
    func.closure = currentEnv;
    
    userFunctions[node->name] = func;
}

void Interpreter::visit(ReturnStatement* node) {
    if (node->value) {
        Value val = evaluate(node->value.get());
        ReturnException ret(std::monostate{});
        if (val.isInt()) {
            ret.value = val.asInt();
        } else if (val.isFloat()) {
            ret.value = val.asFloat();
        } else if (val.isString()) {
            ret.value = val.asString();
        } else if (val.isBool()) {
            ret.value = val.asBool();
        }
        throw ret;
    }
    throw ReturnException(std::monostate{});
}

void Interpreter::visit(TryStatement* node) {
    try {
        // Execute try block
        if (node->tryBlock) {
            node->tryBlock->accept(*this);
        }
    } catch (const std::exception& e) {
        // Create catch environment with error variable
        auto catchEnv = std::make_shared<Environment>(currentEnv);
        catchEnv->define(node->errorVariable, Value(std::string(e.what())));
        
        auto prevEnv = currentEnv;
        currentEnv = catchEnv;
        
        // Execute catch block
        if (node->catchBlock) {
            node->catchBlock->accept(*this);
        }
        
        currentEnv = prevEnv;
    }
}
