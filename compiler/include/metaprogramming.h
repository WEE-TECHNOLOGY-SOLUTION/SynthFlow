#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <variant>
#include <any>

// ===== SynthFlow Metaprogramming System =====
// Provides compile-time code generation and reflection capabilities

// Forward declarations
class ASTNode;
class Expression;
class Statement;

// ===== Macro System =====

// Macro parameter
struct MacroParam {
    std::string name;
    bool isVariadic;      // ...args
    bool hasDefault;
    std::string defaultValue;
    
    MacroParam() : isVariadic(false), hasDefault(false) {}
    MacroParam(const std::string& n) : name(n), isVariadic(false), hasDefault(false) {}
};

// Macro definition
struct MacroDef {
    std::string name;
    std::vector<MacroParam> params;
    std::string body;           // Template string with placeholders
    bool isHygienic;            // Whether macro is hygienic (avoids capture)
    
    MacroDef() : isHygienic(true) {}
    MacroDef(const std::string& n) : name(n), isHygienic(true) {}
    
    std::string expand(const std::vector<std::string>& args) const {
        std::string result = body;
        
        // Replace placeholders with arguments
        for (size_t i = 0; i < params.size() && i < args.size(); ++i) {
            std::string placeholder = "${" + params[i].name + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), args[i]);
                pos += args[i].length();
            }
        }
        
        return result;
    }
};

// Macro registry
class MacroRegistry {
private:
    std::map<std::string, MacroDef> macros;
    
public:
    void define(const MacroDef& macro) {
        macros[macro.name] = macro;
    }
    
    bool has(const std::string& name) const {
        return macros.find(name) != macros.end();
    }
    
    const MacroDef* get(const std::string& name) const {
        auto it = macros.find(name);
        return it != macros.end() ? &it->second : nullptr;
    }
    
    std::string expand(const std::string& name, 
                       const std::vector<std::string>& args) const {
        const MacroDef* macro = get(name);
        if (!macro) return "";
        return macro->expand(args);
    }
};

// ===== Reflection System =====

// Type information at runtime
struct TypeInfo {
    std::string name;
    std::string kind;         // "struct", "function", "primitive", etc.
    size_t size;
    std::vector<std::string> fieldNames;
    std::vector<std::string> fieldTypes;
    std::vector<std::string> methodNames;
    
    TypeInfo() : size(0) {}
    TypeInfo(const std::string& n, const std::string& k)
        : name(n), kind(k), size(0) {}
    
    bool hasField(const std::string& name) const {
        for (const auto& f : fieldNames) {
            if (f == name) return true;
        }
        return false;
    }
    
    bool hasMethod(const std::string& name) const {
        for (const auto& m : methodNames) {
            if (m == name) return true;
        }
        return false;
    }
};

// Reflection registry
class ReflectionRegistry {
private:
    std::map<std::string, TypeInfo> types;
    
public:
    void registerType(const TypeInfo& info) {
        types[info.name] = info;
    }
    
    const TypeInfo* getType(const std::string& name) const {
        auto it = types.find(name);
        return it != types.end() ? &it->second : nullptr;
    }
    
    bool hasType(const std::string& name) const {
        return types.find(name) != types.end();
    }
    
    std::vector<std::string> getAllTypes() const {
        std::vector<std::string> result;
        for (const auto& pair : types) {
            result.push_back(pair.first);
        }
        return result;
    }
    
    void initBuiltinTypes() {
        registerType(TypeInfo("int", "primitive"));
        registerType(TypeInfo("float", "primitive"));
        registerType(TypeInfo("string", "primitive"));
        registerType(TypeInfo("bool", "primitive"));
        registerType(TypeInfo("array", "collection"));
        registerType(TypeInfo("function", "callable"));
    }
};

// ===== Code Generation =====

// Code template with placeholders
class CodeTemplate {
private:
    std::string template_;
    
public:
    CodeTemplate() {}
    CodeTemplate(const std::string& tmpl) : template_(tmpl) {}
    
    std::string generate(const std::map<std::string, std::string>& values) const {
        std::string result = template_;
        
        for (const auto& pair : values) {
            std::string placeholder = "{{" + pair.first + "}}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), pair.second);
                pos += pair.second.length();
            }
        }
        
        return result;
    }
    
    // Generate with repeat for lists
    std::string generateWithRepeat(
        const std::string& repeatMarker,
        const std::vector<std::map<std::string, std::string>>& items
    ) const {
        std::string result;
        
        for (const auto& item : items) {
            std::string part = template_;
            for (const auto& pair : item) {
                std::string placeholder = "{{" + pair.first + "}}";
                size_t pos = 0;
                while ((pos = part.find(placeholder, pos)) != std::string::npos) {
                    part.replace(pos, placeholder.length(), pair.second);
                    pos += pair.second.length();
                }
            }
            result += part + "\n";
        }
        
        return result;
    }
};

// ===== Decorator System =====

// Decorator function type
using DecoratorFunc = std::function<std::string(const std::string&)>;

// Decorator definition
struct Decorator {
    std::string name;
    std::vector<std::string> args;
    DecoratorFunc transform;
    
    Decorator() {}
    Decorator(const std::string& n) : name(n) {}
};

// Decorator registry
class DecoratorRegistry {
private:
    std::map<std::string, Decorator> decorators;
    
public:
    void registerDecorator(const std::string& name, DecoratorFunc func) {
        Decorator dec(name);
        dec.transform = std::move(func);
        decorators[name] = dec;
    }
    
    std::string apply(const std::string& name, const std::string& code) const {
        auto it = decorators.find(name);
        if (it == decorators.end()) return code;
        return it->second.transform(code);
    }
    
    bool has(const std::string& name) const {
        return decorators.find(name) != decorators.end();
    }
    
    void initBuiltinDecorators() {
        // @deprecated - adds a warning print
        registerDecorator("deprecated", [](const std::string& code) {
            return "print(\"[DEPRECATED] This function is deprecated\")\n" + code;
        });
        
        // @log - adds logging
        registerDecorator("log", [](const std::string& code) {
            return "print(\"[LOG] Function called\")\n" + code;
        });
        
        // @benchmark - adds timing
        registerDecorator("benchmark", [](const std::string& code) {
            return "let _start = now()\n" + code + 
                   "\nprint(\"[BENCHMARK] Execution time: \" + str(now() - _start))";
        });
        
        // @memoize - would add caching (conceptual)
        registerDecorator("memoize", [](const std::string& code) {
            return "// Memoization enabled\n" + code;
        });
    }
};

// ===== Compile-Time Evaluation =====

// Compile-time constant expression
struct ConstExpr {
    std::variant<int64_t, double, std::string, bool> value;
    std::string type;
    
    ConstExpr() : type("unknown") {}
    
    template<typename T>
    ConstExpr(T val) : value(val) {
        if constexpr (std::is_same_v<T, int64_t>) type = "int";
        else if constexpr (std::is_same_v<T, double>) type = "float";
        else if constexpr (std::is_same_v<T, std::string>) type = "string";
        else if constexpr (std::is_same_v<T, bool>) type = "bool";
    }
    
    bool isInt() const { return std::holds_alternative<int64_t>(value); }
    bool isFloat() const { return std::holds_alternative<double>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isBool() const { return std::holds_alternative<bool>(value); }
    
    int64_t asInt() const { return std::get<int64_t>(value); }
    double asFloat() const { return std::get<double>(value); }
    const std::string& asString() const { return std::get<std::string>(value); }
    bool asBool() const { return std::get<bool>(value); }
};

// Compile-time evaluator
class ConstEvaluator {
public:
    // Evaluate binary expression at compile time
    static ConstExpr evalBinary(const std::string& op, 
                                const ConstExpr& left, 
                                const ConstExpr& right) {
        if (left.isInt() && right.isInt()) {
            int64_t l = left.asInt();
            int64_t r = right.asInt();
            
            if (op == "+") return ConstExpr(l + r);
            if (op == "-") return ConstExpr(l - r);
            if (op == "*") return ConstExpr(l * r);
            if (op == "/" && r != 0) return ConstExpr(l / r);
            if (op == "%") return ConstExpr(l % r);
            if (op == "==") return ConstExpr(l == r);
            if (op == "!=") return ConstExpr(l != r);
            if (op == "<") return ConstExpr(l < r);
            if (op == ">") return ConstExpr(l > r);
            if (op == "<=") return ConstExpr(l <= r);
            if (op == ">=") return ConstExpr(l >= r);
        }
        
        if (left.isFloat() || right.isFloat()) {
            double l = left.isFloat() ? left.asFloat() : static_cast<double>(left.asInt());
            double r = right.isFloat() ? right.asFloat() : static_cast<double>(right.asInt());
            
            if (op == "+") return ConstExpr(l + r);
            if (op == "-") return ConstExpr(l - r);
            if (op == "*") return ConstExpr(l * r);
            if (op == "/" && r != 0) return ConstExpr(l / r);
        }
        
        if (left.isString() && right.isString() && op == "+") {
            return ConstExpr(left.asString() + right.asString());
        }
        
        if (left.isBool() && right.isBool()) {
            if (op == "&&") return ConstExpr(left.asBool() && right.asBool());
            if (op == "||") return ConstExpr(left.asBool() || right.asBool());
        }
        
        return ConstExpr();
    }
    
    // Evaluate unary expression at compile time
    static ConstExpr evalUnary(const std::string& op, const ConstExpr& operand) {
        if (op == "-" && operand.isInt()) {
            return ConstExpr(-operand.asInt());
        }
        if (op == "-" && operand.isFloat()) {
            return ConstExpr(-operand.asFloat());
        }
        if (op == "!" && operand.isBool()) {
            return ConstExpr(!operand.asBool());
        }
        return ConstExpr();
    }
};

// ===== AST Quote/Unquote (for hygienic macros) =====

// Quoted AST node - represents code as data
struct QuotedAST {
    std::string originalCode;
    std::vector<std::string> freeVariables;  // Variables that need binding
    std::vector<std::string> unquoted;       // Expressions to splice in
    
    QuotedAST() {}
    QuotedAST(const std::string& code) : originalCode(code) {}
    
    std::string splice(const std::map<std::string, std::string>& bindings) const {
        std::string result = originalCode;
        for (const auto& pair : bindings) {
            std::string marker = "$" + pair.first;
            size_t pos = 0;
            while ((pos = result.find(marker, pos)) != std::string::npos) {
                result.replace(pos, marker.length(), pair.second);
                pos += pair.second.length();
            }
        }
        return result;
    }
};

// ===== Metaprogramming Context =====

// Central context for all metaprogramming features
class MetaContext {
public:
    MacroRegistry macros;
    ReflectionRegistry reflection;
    DecoratorRegistry decorators;
    
    MetaContext() {
        reflection.initBuiltinTypes();
        decorators.initBuiltinDecorators();
    }
    
    // Define a macro
    void defineMacro(const std::string& name, 
                     const std::vector<std::string>& params, 
                     const std::string& body) {
        MacroDef macro(name);
        for (const auto& p : params) {
            macro.params.push_back(MacroParam(p));
        }
        macro.body = body;
        macros.define(macro);
    }
    
    // Expand a macro
    std::string expandMacro(const std::string& name,
                            const std::vector<std::string>& args) {
        return macros.expand(name, args);
    }
    
    // Get type info
    const TypeInfo* getTypeInfo(const std::string& typeName) const {
        return reflection.getType(typeName);
    }
    
    // Apply decorator
    std::string applyDecorator(const std::string& name, const std::string& code) {
        return decorators.apply(name, code);
    }
};

// Global metaprogramming context
inline MetaContext& getMetaContext() {
    static MetaContext instance;
    return instance;
}

// ===== Example Usage in SynthFlow =====
// 
// // Macro definition
// macro debug(expr) {
//     print("[DEBUG] ${expr} = " + str(${expr}))
// }
// 
// // Usage
// debug(x + y)  // Expands to: print("[DEBUG] x + y = " + str(x + y))
// 
// // Decorator
// @deprecated
// fn oldFunction() { ... }
// 
// // Reflection
// let info = typeOf(myStruct)
// print(info.fields)  // ["x", "y", "z"]
// 
// // Code generation
// @generate struct Vector3 { x: float, y: float, z: float }
