#pragma once
#include "types.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

// Forward declarations
class Expression;
class Statement;

// ===== Struct/Class System =====

// Field definition
struct Field {
    std::string name;
    std::shared_ptr<Type> type;
    std::unique_ptr<Expression> defaultValue;  // Optional default
    bool isPublic;
    bool isMutable;
    
    Field() : isPublic(true), isMutable(true) {}
    Field(const std::string& n, std::shared_ptr<Type> t) 
        : name(n), type(t), isPublic(true), isMutable(true) {}
};

// Method definition
struct Method {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> parameters;
    std::shared_ptr<Type> returnType;
    std::unique_ptr<Statement> body;  // BlockStatement
    bool isPublic;
    bool isStatic;
    bool isVirtual;
    
    Method() : isPublic(true), isStatic(false), isVirtual(false) {}
    Method(const std::string& n) : name(n), isPublic(true), isStatic(false), isVirtual(false) {}
};

// Struct type definition
struct StructDef {
    std::string name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<std::string> implementedTraits;
    
    StructDef() {}
    StructDef(const std::string& n) : name(n) {}
    
    void addField(const Field& field) {
        fields.push_back(field);
    }
    
    void addMethod(Method&& method) {
        methods.push_back(std::move(method));
    }
    
    Field* getField(const std::string& name) {
        for (auto& field : fields) {
            if (field.name == name) return &field;
        }
        return nullptr;
    }
    
    Method* getMethod(const std::string& name) {
        for (auto& method : methods) {
            if (method.name == name) return &method;
        }
        return nullptr;
    }
    
    std::shared_ptr<Type> toType() const {
        return std::make_shared<Type>(TypeKind::STRUCT, name);
    }
};

// ===== Trait System =====

// Trait method signature
struct TraitMethod {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> parameters;
    std::shared_ptr<Type> returnType;
    bool hasDefaultImpl;
    std::unique_ptr<Statement> defaultImpl;  // Optional default implementation
    
    TraitMethod() : hasDefaultImpl(false) {}
    TraitMethod(const std::string& n) : name(n), hasDefaultImpl(false) {}
};

// Trait definition
struct TraitDef {
    std::string name;
    std::vector<TraitMethod> methods;
    std::vector<std::string> superTraits;  // Traits this trait extends
    
    TraitDef() {}
    TraitDef(const std::string& n) : name(n) {}
    
    void addMethod(TraitMethod&& method) {
        methods.push_back(std::move(method));
    }
    
    bool hasMethod(const std::string& name) const {
        for (const auto& method : methods) {
            if (method.name == name) return true;
        }
        return false;
    }
    
    const TraitMethod* getMethod(const std::string& name) const {
        for (const auto& method : methods) {
            if (method.name == name) return &method;
        }
        return nullptr;
    }
};

// Trait implementation for a type
struct TraitImpl {
    std::string traitName;
    std::string typeName;
    std::vector<Method> methods;  // Implemented methods
    
    TraitImpl() {}
    TraitImpl(const std::string& trait, const std::string& type) 
        : traitName(trait), typeName(type) {}
};

// ===== Type Registry =====

// Registry for user-defined types
class TypeRegistry {
private:
    std::map<std::string, StructDef> structs;
    std::map<std::string, TraitDef> traits;
    std::map<std::string, std::vector<TraitImpl>> traitImpls;  // type -> implementations
    
public:
    // Struct operations
    void registerStruct(const StructDef& def) {
        structs[def.name] = def;
    }
    
    StructDef* getStruct(const std::string& name) {
        auto it = structs.find(name);
        return it != structs.end() ? &it->second : nullptr;
    }
    
    bool hasStruct(const std::string& name) const {
        return structs.find(name) != structs.end();
    }
    
    // Trait operations
    void registerTrait(const TraitDef& def) {
        traits[def.name] = def;
    }
    
    TraitDef* getTrait(const std::string& name) {
        auto it = traits.find(name);
        return it != traits.end() ? &it->second : nullptr;
    }
    
    bool hasTrait(const std::string& name) const {
        return traits.find(name) != traits.end();
    }
    
    // Trait implementation operations
    void registerImpl(const TraitImpl& impl) {
        traitImpls[impl.typeName].push_back(impl);
    }
    
    bool typeImplementsTrait(const std::string& typeName, const std::string& traitName) const {
        auto it = traitImpls.find(typeName);
        if (it == traitImpls.end()) return false;
        
        for (const auto& impl : it->second) {
            if (impl.traitName == traitName) return true;
        }
        return false;
    }
    
    const TraitImpl* getImpl(const std::string& typeName, const std::string& traitName) const {
        auto it = traitImpls.find(typeName);
        if (it == traitImpls.end()) return nullptr;
        
        for (const auto& impl : it->second) {
            if (impl.traitName == traitName) return &impl;
        }
        return nullptr;
    }
    
    // Get all traits implemented by a type
    std::vector<std::string> getImplementedTraits(const std::string& typeName) const {
        std::vector<std::string> result;
        auto it = traitImpls.find(typeName);
        if (it != traitImpls.end()) {
            for (const auto& impl : it->second) {
                result.push_back(impl.traitName);
            }
        }
        return result;
    }
    
    // Validate trait implementation
    bool validateImpl(const std::string& typeName, const std::string& traitName) const {
        const TraitDef* trait = const_cast<TypeRegistry*>(this)->getTrait(traitName);
        if (!trait) return false;
        
        const TraitImpl* impl = getImpl(typeName, traitName);
        if (!impl) return false;
        
        // Check all required methods are implemented
        for (const auto& traitMethod : trait->methods) {
            if (traitMethod.hasDefaultImpl) continue;  // Has default, optional
            
            bool found = false;
            for (const auto& implMethod : impl->methods) {
                if (implMethod.name == traitMethod.name) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        
        return true;
    }
};

// ===== Instance (Object) =====

// Runtime instance of a struct
struct Instance {
    std::string typeName;
    std::map<std::string, std::shared_ptr<void>> fields;  // Field values
    
    Instance() {}
    Instance(const std::string& type) : typeName(type) {}
    
    template<typename T>
    void setField(const std::string& name, T value) {
        fields[name] = std::make_shared<T>(value);
    }
    
    template<typename T>
    T* getField(const std::string& name) {
        auto it = fields.find(name);
        if (it == fields.end()) return nullptr;
        return static_cast<T*>(it->second.get());
    }
    
    bool hasField(const std::string& name) const {
        return fields.find(name) != fields.end();
    }
};

// ===== AST Nodes for OOP =====
// These would be added to ast.h:

// struct Point { x: float, y: float }
// struct Point {
//     x: float
//     y: float
//     fn distance(other: Point) -> float { ... }
// }

// trait Printable {
//     fn toString() -> string
// }

// impl Printable for Point {
//     fn toString() -> string { ... }
// }

// let p = Point { x: 10.0, y: 20.0 }
// p.x = 15.0
// print(p.distance(other))
