#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>
#include <stdexcept>

// Forward declarations
class Interpreter;
class Environment;

// Runtime exception for control flow
class ReturnException : public std::exception {
public:
    std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<std::variant<std::monostate, int64_t, double, std::string, bool>>> value;
    ReturnException(decltype(value) v) : value(std::move(v)) {}
};

class BreakException : public std::exception {};
class ContinueException : public std::exception {};

// Runtime value type
class Value {
public:
    using ArrayType = std::vector<Value>;
    using FunctionType = std::function<Value(std::vector<Value>&, Interpreter&)>;
    
    std::variant<
        std::monostate,  // null/undefined
        int64_t,         // integer
        double,          // float
        std::string,     // string
        bool,            // boolean
        std::shared_ptr<ArrayType>,     // array
        std::shared_ptr<FunctionType>   // function
    > data;
    
    // Constructors
    Value() : data(std::monostate{}) {}
    Value(int64_t v) : data(v) {}
    Value(int v) : data(static_cast<int64_t>(v)) {}
    Value(double v) : data(v) {}
    Value(const std::string& v) : data(v) {}
    Value(const char* v) : data(std::string(v)) {}
    Value(bool v) : data(v) {}
    Value(std::shared_ptr<ArrayType> v) : data(v) {}
    Value(std::shared_ptr<FunctionType> v) : data(v) {}
    
    // Type checks
    bool isNull() const { return std::holds_alternative<std::monostate>(data); }
    bool isInt() const { return std::holds_alternative<int64_t>(data); }
    bool isFloat() const { return std::holds_alternative<double>(data); }
    bool isString() const { return std::holds_alternative<std::string>(data); }
    bool isBool() const { return std::holds_alternative<bool>(data); }
    bool isArray() const { return std::holds_alternative<std::shared_ptr<ArrayType>>(data); }
    bool isFunction() const { return std::holds_alternative<std::shared_ptr<FunctionType>>(data); }
    bool isNumber() const { return isInt() || isFloat(); }
    
    // Getters
    int64_t asInt() const { return std::get<int64_t>(data); }
    double asFloat() const { 
        if (isInt()) return static_cast<double>(asInt());
        return std::get<double>(data); 
    }
    const std::string& asString() const { return std::get<std::string>(data); }
    bool asBool() const { return std::get<bool>(data); }
    std::shared_ptr<ArrayType> asArray() const { return std::get<std::shared_ptr<ArrayType>>(data); }
    std::shared_ptr<FunctionType> asFunction() const { return std::get<std::shared_ptr<FunctionType>>(data); }
    
    // Convert to string for printing
    std::string toString() const;
    
    // Truthiness
    bool isTruthy() const;
};

// Environment for variable scoping
class Environment {
private:
    std::map<std::string, Value> variables;
    std::shared_ptr<Environment> parent;
    
public:
    Environment() : parent(nullptr) {}
    Environment(std::shared_ptr<Environment> p) : parent(p) {}
    
    void define(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    void set(const std::string& name, const Value& value);
    bool exists(const std::string& name) const;
};

// User-defined function wrapper
struct UserFunction {
    std::vector<std::string> parameters;
    BlockStatement* body;
    std::shared_ptr<Environment> closure;
};

// Interpreter class
class Interpreter : public ASTVisitor {
private:
    std::shared_ptr<Environment> globalEnv;
    std::shared_ptr<Environment> currentEnv;
    Value lastValue;
    
    // Store user functions
    std::map<std::string, UserFunction> userFunctions;
    
public:
    Interpreter();
    
    // Execute statements
    void execute(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Get result of last expression
    Value getLastValue() const { return lastValue; }
    
    // Call a function
    Value callFunction(const std::string& name, std::vector<Value>& args);
    
    // Environment access
    std::shared_ptr<Environment> getGlobalEnv() { return globalEnv; }
    std::shared_ptr<Environment> getCurrentEnv() { return currentEnv; }
    void setCurrentEnv(std::shared_ptr<Environment> env) { currentEnv = env; }
    
    // Expression visitors
    void visit(IntegerLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(NullLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(AssignmentExpression* node) override;
    void visit(CallExpression* node) override;
    void visit(ArrayLiteral* node) override;
    void visit(ArrayIndexExpression* node) override;
    void visit(ArrayAssignmentExpression* node) override;
    
    // Statement visitors
    void visit(VariableDeclaration* node) override;
    void visit(ExpressionStatement* node) override;
    void visit(BlockStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ForStatement* node) override;
    void visit(BreakStatement* node) override;
    void visit(ContinueStatement* node) override;
    void visit(FunctionDeclaration* node) override;
    void visit(ReturnStatement* node) override;
    void visit(TryStatement* node) override;
    
private:
    // Helper to evaluate an expression
    Value evaluate(Expression* expr);
    
    // Register built-in functions
    void registerBuiltins();
    
    // Track const variables
    std::map<std::string, bool> constVariables;
};

#endif // INTERPRETER_H
