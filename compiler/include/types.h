#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <variant>

// Forward declaration
class Expression;
class Statement;

// ===== Type Kinds =====
enum class TypeKind {
    UNKNOWN,     // Type not yet determined
    VOID,        // No return value
    INT,         // Integer type
    FLOAT,       // Floating point type
    STRING,      // String type
    BOOL,        // Boolean type
    ARRAY,       // Array type
    FUNCTION,    // Function type
    NULLABLE,    // Nullable wrapper (T?)
    GENERIC,     // Generic type parameter (T)
    UNION,       // Union of types (A | B)
    STRUCT,      // User-defined struct
    CLASS,       // User-defined class
    ANY          // Any type (for gradual typing)
};

// ===== Type Class =====
class Type {
public:
    TypeKind kind;
    std::string name;  // Type name (for named types)
    
    // For array types: the element type
    std::shared_ptr<Type> elementType;
    
    // For function types: parameter types and return type
    std::vector<std::shared_ptr<Type>> paramTypes;
    std::shared_ptr<Type> returnType;
    
    // For nullable types: the inner type
    std::shared_ptr<Type> innerType;
    
    // For generic types: constraints
    std::vector<std::shared_ptr<Type>> constraints;
    
    // For union types: member types
    std::vector<std::shared_ptr<Type>> unionMembers;
    
    // Whether this type is nullable
    bool isNullable = false;
    
    // Constructors
    Type() : kind(TypeKind::UNKNOWN) {}
    Type(TypeKind k) : kind(k) {}
    Type(TypeKind k, const std::string& n) : kind(k), name(n) {}
    
    // Static factory methods
    static std::shared_ptr<Type> makeInt() {
        return std::make_shared<Type>(TypeKind::INT, "int");
    }
    
    static std::shared_ptr<Type> makeFloat() {
        return std::make_shared<Type>(TypeKind::FLOAT, "float");
    }
    
    static std::shared_ptr<Type> makeString() {
        return std::make_shared<Type>(TypeKind::STRING, "string");
    }
    
    static std::shared_ptr<Type> makeBool() {
        return std::make_shared<Type>(TypeKind::BOOL, "bool");
    }
    
    static std::shared_ptr<Type> makeVoid() {
        return std::make_shared<Type>(TypeKind::VOID, "void");
    }
    
    static std::shared_ptr<Type> makeAny() {
        return std::make_shared<Type>(TypeKind::ANY, "any");
    }
    
    static std::shared_ptr<Type> makeUnknown() {
        return std::make_shared<Type>(TypeKind::UNKNOWN);
    }
    
    static std::shared_ptr<Type> makeArray(std::shared_ptr<Type> elemType) {
        auto t = std::make_shared<Type>(TypeKind::ARRAY);
        t->elementType = elemType;
        t->name = elemType->toString() + "[]";
        return t;
    }
    
    static std::shared_ptr<Type> makeNullable(std::shared_ptr<Type> inner) {
        auto t = std::make_shared<Type>(TypeKind::NULLABLE);
        t->innerType = inner;
        t->isNullable = true;
        t->name = inner->toString() + "?";
        return t;
    }
    
    static std::shared_ptr<Type> makeFunction(
        std::vector<std::shared_ptr<Type>> params,
        std::shared_ptr<Type> ret
    ) {
        auto t = std::make_shared<Type>(TypeKind::FUNCTION);
        t->paramTypes = std::move(params);
        t->returnType = ret;
        return t;
    }
    
    static std::shared_ptr<Type> makeGeneric(const std::string& name) {
        return std::make_shared<Type>(TypeKind::GENERIC, name);
    }
    
    static std::shared_ptr<Type> makeUnion(std::vector<std::shared_ptr<Type>> members) {
        auto t = std::make_shared<Type>(TypeKind::UNION);
        t->unionMembers = std::move(members);
        return t;
    }
    
    // Type to string conversion
    std::string toString() const {
        if (!name.empty()) return name;
        
        switch (kind) {
            case TypeKind::UNKNOWN: return "unknown";
            case TypeKind::VOID: return "void";
            case TypeKind::INT: return "int";
            case TypeKind::FLOAT: return "float";
            case TypeKind::STRING: return "string";
            case TypeKind::BOOL: return "bool";
            case TypeKind::ANY: return "any";
            case TypeKind::ARRAY:
                return elementType ? elementType->toString() + "[]" : "array";
            case TypeKind::NULLABLE:
                return innerType ? innerType->toString() + "?" : "nullable";
            case TypeKind::FUNCTION: {
                std::string result = "(";
                for (size_t i = 0; i < paramTypes.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += paramTypes[i]->toString();
                }
                result += ") -> ";
                result += returnType ? returnType->toString() : "void";
                return result;
            }
            case TypeKind::UNION: {
                std::string result;
                for (size_t i = 0; i < unionMembers.size(); ++i) {
                    if (i > 0) result += " | ";
                    result += unionMembers[i]->toString();
                }
                return result;
            }
            default: return "unknown";
        }
    }
    
    // Type equality check
    bool equals(const Type& other) const {
        if (kind != other.kind) return false;
        
        switch (kind) {
            case TypeKind::ARRAY:
                return elementType && other.elementType && 
                       elementType->equals(*other.elementType);
            case TypeKind::NULLABLE:
                return innerType && other.innerType && 
                       innerType->equals(*other.innerType);
            case TypeKind::FUNCTION:
                if (paramTypes.size() != other.paramTypes.size()) return false;
                for (size_t i = 0; i < paramTypes.size(); ++i) {
                    if (!paramTypes[i]->equals(*other.paramTypes[i])) return false;
                }
                return returnType && other.returnType && 
                       returnType->equals(*other.returnType);
            case TypeKind::GENERIC:
            case TypeKind::STRUCT:
            case TypeKind::CLASS:
                return name == other.name;
            default:
                return true;
        }
    }
    
    // Check if this type is assignable from another type
    bool isAssignableFrom(const Type& other) const {
        // Any accepts everything
        if (kind == TypeKind::ANY) return true;
        
        // Unknown can be assigned anything (gradual typing)
        if (kind == TypeKind::UNKNOWN) return true;
        
        // Exact match
        if (equals(other)) return true;
        
        // Nullable accepts null or the inner type
        if (kind == TypeKind::NULLABLE) {
            if (other.kind == TypeKind::NULLABLE) {
                return innerType->isAssignableFrom(*other.innerType);
            }
            return innerType->isAssignableFrom(other);
        }
        
        // Int can be promoted to float
        if (kind == TypeKind::FLOAT && other.kind == TypeKind::INT) {
            return true;
        }
        
        // Union types
        if (kind == TypeKind::UNION) {
            for (const auto& member : unionMembers) {
                if (member->isAssignableFrom(other)) return true;
            }
            return false;
        }
        
        return false;
    }
};

// ===== Type Environment =====
class TypeEnvironment {
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> bindings;
    std::shared_ptr<TypeEnvironment> parent;
    
public:
    TypeEnvironment() : parent(nullptr) {}
    TypeEnvironment(std::shared_ptr<TypeEnvironment> p) : parent(p) {}
    
    void define(const std::string& name, std::shared_ptr<Type> type) {
        bindings[name] = type;
    }
    
    std::shared_ptr<Type> lookup(const std::string& name) const {
        auto it = bindings.find(name);
        if (it != bindings.end()) {
            return it->second;
        }
        if (parent) {
            return parent->lookup(name);
        }
        return nullptr;
    }
    
    bool has(const std::string& name) const {
        return lookup(name) != nullptr;
    }
    
    std::shared_ptr<TypeEnvironment> extend() {
        return std::make_shared<TypeEnvironment>(
            std::shared_ptr<TypeEnvironment>(this, [](auto*){})
        );
    }
};

// ===== Type Inference Engine =====
class TypeInference {
private:
    std::shared_ptr<TypeEnvironment> globalEnv;
    std::shared_ptr<TypeEnvironment> currentEnv;
    std::vector<std::string> errors;
    
    // Type variable counter for inference
    int typeVarCounter = 0;
    
    // Substitution map for type variables
    std::unordered_map<std::string, std::shared_ptr<Type>> substitutions;
    
public:
    TypeInference() {
        globalEnv = std::make_shared<TypeEnvironment>();
        currentEnv = globalEnv;
        registerBuiltins();
    }
    
    void registerBuiltins() {
        // Register built-in functions
        globalEnv->define("print", Type::makeFunction({Type::makeAny()}, Type::makeVoid()));
        globalEnv->define("input", Type::makeFunction({Type::makeString()}, Type::makeString()));
        globalEnv->define("len", Type::makeFunction({Type::makeAny()}, Type::makeInt()));
        globalEnv->define("str", Type::makeFunction({Type::makeAny()}, Type::makeString()));
        globalEnv->define("int", Type::makeFunction({Type::makeAny()}, Type::makeInt()));
        globalEnv->define("float", Type::makeFunction({Type::makeAny()}, Type::makeFloat()));
        globalEnv->define("read_file", Type::makeFunction({Type::makeString()}, Type::makeString()));
        globalEnv->define("write_file", Type::makeFunction(
            {Type::makeString(), Type::makeString()}, Type::makeVoid()));
    }
    
    // Generate fresh type variable
    std::shared_ptr<Type> freshTypeVar() {
        std::string name = "T" + std::to_string(typeVarCounter++);
        return Type::makeGeneric(name);
    }
    
    // Unify two types (for type inference)
    bool unify(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2) {
        // Apply current substitutions
        t1 = applySubstitutions(t1);
        t2 = applySubstitutions(t2);
        
        // Same type - success
        if (t1->equals(*t2)) return true;
        
        // Type variable - bind it
        if (t1->kind == TypeKind::GENERIC) {
            substitutions[t1->name] = t2;
            return true;
        }
        if (t2->kind == TypeKind::GENERIC) {
            substitutions[t2->name] = t1;
            return true;
        }
        
        // Unknown can be unified with anything
        if (t1->kind == TypeKind::UNKNOWN || t2->kind == TypeKind::UNKNOWN) {
            return true;
        }
        
        // Array types
        if (t1->kind == TypeKind::ARRAY && t2->kind == TypeKind::ARRAY) {
            return unify(t1->elementType, t2->elementType);
        }
        
        // Function types
        if (t1->kind == TypeKind::FUNCTION && t2->kind == TypeKind::FUNCTION) {
            if (t1->paramTypes.size() != t2->paramTypes.size()) return false;
            for (size_t i = 0; i < t1->paramTypes.size(); ++i) {
                if (!unify(t1->paramTypes[i], t2->paramTypes[i])) return false;
            }
            return unify(t1->returnType, t2->returnType);
        }
        
        // Numeric promotion
        if ((t1->kind == TypeKind::INT && t2->kind == TypeKind::FLOAT) ||
            (t1->kind == TypeKind::FLOAT && t2->kind == TypeKind::INT)) {
            return true;
        }
        
        return false;
    }
    
    // Apply substitutions to a type
    std::shared_ptr<Type> applySubstitutions(std::shared_ptr<Type> type) {
        if (type->kind == TypeKind::GENERIC) {
            auto it = substitutions.find(type->name);
            if (it != substitutions.end()) {
                return applySubstitutions(it->second);
            }
            return type;
        }
        
        if (type->kind == TypeKind::ARRAY) {
            auto newElem = applySubstitutions(type->elementType);
            return Type::makeArray(newElem);
        }
        
        if (type->kind == TypeKind::FUNCTION) {
            std::vector<std::shared_ptr<Type>> newParams;
            for (const auto& param : type->paramTypes) {
                newParams.push_back(applySubstitutions(param));
            }
            auto newRet = applySubstitutions(type->returnType);
            return Type::makeFunction(newParams, newRet);
        }
        
        return type;
    }
    
    // Type check and infer (placeholder for full implementation)
    std::shared_ptr<Type> inferExpression(Expression* expr) {
        // Implementation would walk the AST and infer types
        return Type::makeUnknown();
    }
    
    // Get type from annotation string
    static std::shared_ptr<Type> fromAnnotation(const std::string& annotation) {
        if (annotation.empty()) return Type::makeUnknown();
        
        // Check for nullable
        bool nullable = false;
        std::string typeName = annotation;
        if (!typeName.empty() && typeName.back() == '?') {
            nullable = true;
            typeName = typeName.substr(0, typeName.length() - 1);
        }
        
        // Check for array
        if (typeName.length() > 2 && 
            typeName.substr(typeName.length() - 2) == "[]") {
            std::string elemTypeName = typeName.substr(0, typeName.length() - 2);
            auto elemType = fromAnnotation(elemTypeName);
            auto arrayType = Type::makeArray(elemType);
            if (nullable) return Type::makeNullable(arrayType);
            return arrayType;
        }
        
        // Basic types
        std::shared_ptr<Type> result;
        if (typeName == "int") result = Type::makeInt();
        else if (typeName == "float") result = Type::makeFloat();
        else if (typeName == "string") result = Type::makeString();
        else if (typeName == "bool") result = Type::makeBool();
        else if (typeName == "void") result = Type::makeVoid();
        else if (typeName == "any") result = Type::makeAny();
        else result = std::make_shared<Type>(TypeKind::STRUCT, typeName);
        
        if (nullable) return Type::makeNullable(result);
        return result;
    }
    
    // Error handling
    void reportError(const std::string& message) {
        errors.push_back(message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // Environment access
    std::shared_ptr<TypeEnvironment> getGlobalEnv() { return globalEnv; }
    std::shared_ptr<TypeEnvironment> getCurrentEnv() { return currentEnv; }
    void pushScope() { currentEnv = currentEnv->extend(); }
    void popScope() { 
        // Would need proper parent management
    }
};

// ===== Type Checker =====
class TypeChecker {
private:
    TypeInference inference;
    std::vector<std::string> errors;
    
public:
    TypeChecker() {}
    
    // Check type compatibility for assignment
    bool checkAssignment(std::shared_ptr<Type> target, std::shared_ptr<Type> value) {
        return target->isAssignableFrom(*value);
    }
    
    // Check function call arguments
    bool checkFunctionCall(
        std::shared_ptr<Type> funcType,
        const std::vector<std::shared_ptr<Type>>& argTypes
    ) {
        if (funcType->kind != TypeKind::FUNCTION) {
            reportError("Not a function type");
            return false;
        }
        
        if (funcType->paramTypes.size() != argTypes.size()) {
            reportError("Wrong number of arguments: expected " + 
                       std::to_string(funcType->paramTypes.size()) + 
                       ", got " + std::to_string(argTypes.size()));
            return false;
        }
        
        for (size_t i = 0; i < argTypes.size(); ++i) {
            if (!funcType->paramTypes[i]->isAssignableFrom(*argTypes[i])) {
                reportError("Type mismatch for argument " + std::to_string(i + 1));
                return false;
            }
        }
        
        return true;
    }
    
    // Check binary operation
    std::shared_ptr<Type> checkBinaryOp(
        const std::string& op,
        std::shared_ptr<Type> left,
        std::shared_ptr<Type> right
    ) {
        // Arithmetic operators
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            if (left->kind == TypeKind::STRING && op == "+") {
                // String concatenation
                return Type::makeString();
            }
            
            if ((left->kind == TypeKind::INT || left->kind == TypeKind::FLOAT) &&
                (right->kind == TypeKind::INT || right->kind == TypeKind::FLOAT)) {
                // Numeric operation
                if (left->kind == TypeKind::FLOAT || right->kind == TypeKind::FLOAT) {
                    return Type::makeFloat();
                }
                return Type::makeInt();
            }
            
            reportError("Invalid operand types for " + op);
            return Type::makeUnknown();
        }
        
        // Comparison operators
        if (op == "==" || op == "!=" || op == "<" || op == ">" || 
            op == "<=" || op == ">=") {
            return Type::makeBool();
        }
        
        // Logical operators
        if (op == "&&" || op == "||") {
            return Type::makeBool();
        }
        
        return Type::makeUnknown();
    }
    
    // Error handling
    void reportError(const std::string& message) {
        errors.push_back(message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
};
