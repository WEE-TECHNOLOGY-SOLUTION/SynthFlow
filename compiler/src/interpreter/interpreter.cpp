#include "../../include/interpreter.h"
#include "../../include/http_client.h"
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
    if (isMap()) {
        std::string result = "{";
        auto m = asMap();
        bool first = true;
        for (const auto& [key, val] : *m) {
            if (!first) result += ", ";
            result += "\"" + key + "\": " + val.toString();
            first = false;
        }
        result += "}";
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
                    return Value(static_cast<int64_t>(std::stoll(args[0].asString())));
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
    
    // ===== Gemini API Built-in Functions =====
    
    // gemini_set_api_key(key) - Set the Gemini API key
    globalEnv->define("gemini_set_api_key", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("gemini_set_api_key() requires a string API key");
            }
            http::gemini::setApiKey(args[0].asString());
            return Value(true);
        }
    )));
    
    // gemini_has_api_key() - Check if API key is set
    globalEnv->define("gemini_has_api_key", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            return Value(http::gemini::hasApiKey());
        }
    )));
    
    // gemini_complete(prompt) - Generate text from prompt
    globalEnv->define("gemini_complete", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("gemini_complete() requires a prompt string");
            }
            std::string model = "gemini-2.0-flash";
            if (args.size() > 1 && args[1].isString()) {
                model = args[1].asString();
            }
            std::string result = http::gemini::generateContent(args[0].asString(), model);
            return Value(result);
        }
    )));
    
    // gemini_chat(systemPrompt, userMessage) - Chat with system instruction
    globalEnv->define("gemini_chat", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("gemini_chat() requires systemPrompt and userMessage strings");
            }
            std::string model = "gemini-2.0-flash";
            if (args.size() > 2 && args[2].isString()) {
                model = args[2].asString();
            }
            std::string result = http::gemini::generateContentWithSystem(
                args[0].asString(),  // system instruction
                args[1].asString(),  // user prompt
                model
            );
            return Value(result);
        }
    )));
    
    // http_get(url) - Perform HTTP GET request
    globalEnv->define("http_get", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("http_get() requires a URL string");
            }
            http::Client client;
            http::Response response = client.get(args[0].asString());
            
            // Return a map with status, body, and error
            auto resultMap = std::make_shared<Value::MapType>();
            (*resultMap)["status"] = Value(static_cast<int64_t>(response.statusCode));
            (*resultMap)["body"] = Value(response.body);
            (*resultMap)["error"] = Value(response.error);
            return Value(resultMap);
        }
    )));
    
    // http_post(url, body) - Perform HTTP POST request
    globalEnv->define("http_post", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("http_post() requires URL and body strings");
            }
            http::Client client;
            http::Response response = client.post(args[0].asString(), args[1].asString());
            
            // Return a map with status, body, and error
            auto resultMap = std::make_shared<Value::MapType>();
            (*resultMap)["status"] = Value(static_cast<int64_t>(response.statusCode));
            (*resultMap)["body"] = Value(response.body);
            (*resultMap)["error"] = Value(response.error);
            return Value(resultMap);
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

void Interpreter::visit(LambdaExpression* node) {
    // Create a callable value for the lambda
    // For now, store as a string representation
    (void)node;
    lastValue = Value(std::string("<lambda>"));
}

void Interpreter::visit(MatchExpression* node) {
    Value subject = evaluate(node->subject.get());
    
    for (auto& matchCase : node->cases) {
        // nullptr pattern means default case
        if (!matchCase.pattern) {
            lastValue = evaluate(matchCase.result.get());
            return;
        }
        
        Value pattern = evaluate(matchCase.pattern.get());
        
        // Simple equality check
        bool matches = false;
        if (subject.isInt() && pattern.isInt()) {
            matches = subject.asInt() == pattern.asInt();
        } else if (subject.isString() && pattern.isString()) {
            matches = subject.asString() == pattern.asString();
        } else if (subject.isBool() && pattern.isBool()) {
            matches = subject.asBool() == pattern.asBool();
        }
        
        if (matches) {
            lastValue = evaluate(matchCase.result.get());
            return;
        }
    }
    
    // No match found
    lastValue = Value();
}

void Interpreter::visit(CompoundAssignment* node) {
    auto* id = dynamic_cast<Identifier*>(node->target.get());
    if (!id) {
        throw std::runtime_error("Compound assignment target must be an identifier");
    }
    
    Value current = currentEnv->get(id->name);
    Value value = evaluate(node->value.get());
    Value result;
    
    if (node->op == "+=") {
        if (current.isString() || value.isString()) {
            result = Value(current.toString() + value.toString());
        } else if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() + value.asInt());
        } else {
            result = Value(current.asFloat() + value.asFloat());
        }
    } else if (node->op == "-=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() - value.asInt());
        } else {
            result = Value(current.asFloat() - value.asFloat());
        }
    } else if (node->op == "*=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() * value.asInt());
        } else {
            result = Value(current.asFloat() * value.asFloat());
        }
    } else if (node->op == "/=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() / value.asInt());
        } else {
            result = Value(current.asFloat() / value.asFloat());
        }
    }
    
    currentEnv->set(id->name, result);
    lastValue = result;
}

void Interpreter::visit(UpdateExpression* node) {
    auto* id = dynamic_cast<Identifier*>(node->operand.get());
    if (!id) {
        throw std::runtime_error("Update expression operand must be an identifier");
    }
    
    Value current = currentEnv->get(id->name);
    Value result;
    
    if (node->op == "++") {
        if (current.isInt()) {
            result = Value(current.asInt() + 1);
        } else {
            result = Value(current.asFloat() + 1.0);
        }
    } else if (node->op == "--") {
        if (current.isInt()) {
            result = Value(current.asInt() - 1);
        } else {
            result = Value(current.asFloat() - 1.0);
        }
    }
    
    currentEnv->set(id->name, result);
    
    // For prefix, return new value; for postfix, return old value
    lastValue = node->prefix ? result : current;
}

void Interpreter::visit(InterpolatedString* node) {
    std::string result;
    
    for (auto& part : node->parts) {
        if (part.isExpression) {
            Value val = evaluate(part.expr.get());
            result += val.toString();
        } else {
            result += part.text;
        }
    }
    
    lastValue = Value(result);
}

// ========================================
// SADK (Agent Development Kit) Visitors
// ========================================

void Interpreter::visit(MapLiteral* node) {
    auto map = std::make_shared<Value::MapType>();
    
    for (auto& entry : node->entries) {
        // Evaluate key (should be a string or identifier)
        Value keyVal = evaluate(entry.first.get());
        std::string key;
        
        if (keyVal.isString()) {
            key = keyVal.asString();
        } else {
            // For identifiers used as keys, use their name
            if (auto* ident = dynamic_cast<Identifier*>(entry.first.get())) {
                key = ident->name;
            } else {
                key = keyVal.toString();
            }
        }
        
        // Evaluate value
        Value value = evaluate(entry.second.get());
        
        (*map)[key] = value;
    }
    
    lastValue = Value(map);
}

void Interpreter::visit(MemberExpression* node) {
    Value obj = evaluate(node->object.get());
    
    if (obj.isMap()) {
        auto map = obj.asMap();
        auto it = map->find(node->member);
        if (it != map->end()) {
            lastValue = it->second;
        } else {
            throw std::runtime_error("Map does not have member: " + node->member);
        }
    } else if (obj.isArray()) {
        // Array built-in properties
        if (node->member == "length") {
            lastValue = Value(static_cast<int64_t>(obj.asArray()->size()));
        } else {
            throw std::runtime_error("Array does not have member: " + node->member);
        }
    } else if (obj.isString()) {
        // String built-in properties
        if (node->member == "length") {
            lastValue = Value(static_cast<int64_t>(obj.asString().length()));
        } else {
            throw std::runtime_error("String does not have member: " + node->member);
        }
    } else {
        throw std::runtime_error("Cannot access member of non-object type");
    }
}

void Interpreter::visit(SelfExpression* node) {
    (void)node;  // Unused parameter
    // 'self' should be defined in the current environment when inside a method
    if (currentEnv->exists("self")) {
        lastValue = currentEnv->get("self");
    } else {
        throw std::runtime_error("'self' is not defined in current context");
    }
}

void Interpreter::visit(ImportStatement* node) {
    // For now, imports are handled at parse time
    // This is a placeholder that prints a warning if reached at runtime
    std::cerr << "[SADK] Import statement for '" << node->moduleName 
              << "' reached at runtime. Module loading not yet fully implemented." << std::endl;
}

void Interpreter::visit(StructDeclaration* node) {
    // Store struct definition in a special registry
    // For now, we'll create a constructor function for the struct
    
    std::string structName = node->name;
    std::vector<std::string> fieldNames;
    
    for (const auto& field : node->fields) {
        fieldNames.push_back(field.name);
    }
    
    // Create a constructor function that creates map instances
    auto constructor = std::make_shared<Value::FunctionType>(
        [structName, fieldNames](std::vector<Value>& args, Interpreter& interp) -> Value {
            (void)interp;  // Unused
            auto instance = std::make_shared<Value::MapType>();
            
            // Set field values from arguments
            for (size_t i = 0; i < fieldNames.size() && i < args.size(); ++i) {
                (*instance)[fieldNames[i]] = args[i];
            }
            
            // Add a __type__ field to identify the struct type
            (*instance)["__type__"] = Value(structName);
            
            return Value(instance);
        }
    );
    
    // Define the constructor in global environment
    globalEnv->define(structName, Value(constructor));
}
