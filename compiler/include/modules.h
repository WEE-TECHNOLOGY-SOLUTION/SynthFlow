#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

// Forward declarations
class Statement;

// ===== Module System =====
// Handles import/export syntax and module resolution

// Exported symbol
struct ExportedSymbol {
    std::string name;       // Original name
    std::string alias;      // Exported as (if renamed)
    std::string type;       // "function", "variable", "const", "struct"
    
    ExportedSymbol() {}
    ExportedSymbol(const std::string& n, const std::string& t) 
        : name(n), alias(n), type(t) {}
    ExportedSymbol(const std::string& n, const std::string& a, const std::string& t) 
        : name(n), alias(a), type(t) {}
};

// Import specification
struct ImportSpec {
    std::string name;       // Name to import
    std::string alias;      // Local alias (if renamed)
    bool isDefault;         // Is this the default import
    
    ImportSpec() : isDefault(false) {}
    ImportSpec(const std::string& n, bool isDefault_ = false) 
        : name(n), alias(n), isDefault(isDefault_) {}
    ImportSpec(const std::string& n, const std::string& a) 
        : name(n), alias(a), isDefault(false) {}
};

// Module metadata
struct Module {
    std::string path;                           // File path
    std::string name;                           // Module name
    std::vector<ExportedSymbol> exports;        // Exported symbols
    std::vector<std::string> dependencies;     // Required modules
    bool isStdlib;                              // Is this a standard library module
    
    Module() : isStdlib(false) {}
    Module(const std::string& p, const std::string& n) 
        : path(p), name(n), isStdlib(false) {}
    
    void addExport(const ExportedSymbol& sym) {
        exports.push_back(sym);
    }
    
    bool hasExport(const std::string& name) const {
        for (const auto& exp : exports) {
            if (exp.name == name || exp.alias == name) return true;
        }
        return false;
    }
    
    ExportedSymbol* getExport(const std::string& name) {
        for (auto& exp : exports) {
            if (exp.name == name || exp.alias == name) return &exp;
        }
        return nullptr;
    }
};

// Module resolver - finds and loads modules
class ModuleResolver {
private:
    std::vector<std::string> searchPaths;      // Paths to search for modules
    std::map<std::string, Module> loadedModules;
    std::string projectRoot;
    
public:
    ModuleResolver() {
        // Default search paths
        searchPaths.push_back(".");
        searchPaths.push_back("./stdlib");
        searchPaths.push_back("./lib");
    }
    
    void setProjectRoot(const std::string& root) {
        projectRoot = root;
        searchPaths.insert(searchPaths.begin(), root);
        searchPaths.push_back(root + "/stdlib");
        searchPaths.push_back(root + "/lib");
    }
    
    void addSearchPath(const std::string& path) {
        searchPaths.push_back(path);
    }
    
    // Resolve a module path string to an actual file path
    std::string resolveModulePath(const std::string& modulePath, const std::string& fromFile) {
        // Handle stdlib modules (starting with "stdlib/")
        if (modulePath.substr(0, 7) == "stdlib/") {
            std::string stdlibModule = modulePath.substr(7);
            for (const auto& searchPath : searchPaths) {
                std::string fullPath = searchPath + "/" + stdlibModule + ".sf";
                // In real implementation, check if file exists
                return fullPath;
            }
        }
        
        // Handle relative imports (starting with "./" or "../")
        if (modulePath.length() >= 2 && 
            (modulePath.substr(0, 2) == "./" || modulePath.substr(0, 3) == "../")) {
            // Get directory of current file
            size_t lastSlash = fromFile.find_last_of("/\\");
            std::string currentDir = lastSlash != std::string::npos 
                ? fromFile.substr(0, lastSlash) 
                : ".";
            
            // Combine paths (simplified - doesn't handle .. properly)
            std::string relativePath = modulePath;
            if (relativePath.substr(0, 2) == "./") {
                relativePath = relativePath.substr(2);
            }
            
            return currentDir + "/" + relativePath + ".sf";
        }
        
        // Handle absolute module paths
        for (const auto& searchPath : searchPaths) {
            std::string fullPath = searchPath + "/" + modulePath + ".sf";
            return fullPath;  // In real implementation, check existence first
        }
        
        return "";  // Module not found
    }
    
    // Parse exports from a module file (placeholder - would need real parsing)
    Module parseModuleExports(const std::string& filePath) {
        Module mod(filePath, extractModuleName(filePath));
        
        // In real implementation, parse the file and extract exports
        // For now, return empty module
        
        return mod;
    }
    
    // Load a module
    Module* loadModule(const std::string& modulePath, const std::string& fromFile) {
        std::string resolvedPath = resolveModulePath(modulePath, fromFile);
        if (resolvedPath.empty()) {
            return nullptr;
        }
        
        // Check cache
        auto it = loadedModules.find(resolvedPath);
        if (it != loadedModules.end()) {
            return &it->second;
        }
        
        // Parse and cache
        Module mod = parseModuleExports(resolvedPath);
        loadedModules[resolvedPath] = mod;
        return &loadedModules[resolvedPath];
    }
    
    // Get loaded module
    Module* getModule(const std::string& path) {
        auto it = loadedModules.find(path);
        return it != loadedModules.end() ? &it->second : nullptr;
    }
    
    // Clear cache
    void clearCache() {
        loadedModules.clear();
    }
    
private:
    std::string extractModuleName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        std::string filename = lastSlash != std::string::npos 
            ? path.substr(lastSlash + 1) 
            : path;
        
        // Remove .sf extension
        if (filename.length() > 3 && 
            filename.substr(filename.length() - 3) == ".sf") {
            filename = filename.substr(0, filename.length() - 3);
        }
        
        return filename;
    }
};

// Import/Export AST nodes would go in ast.h, but here are their definitions:

// import math from "stdlib/math"
// import { sin, cos } from "stdlib/math"
// import * as math from "stdlib/math"
struct ImportDeclaration {
    std::string modulePath;                 // "stdlib/math"
    std::vector<ImportSpec> imports;        // { sin, cos }
    bool isWildcard;                        // import *
    std::string wildcardAlias;              // as math
    
    ImportDeclaration() : isWildcard(false) {}
};

// export fn calculate() { ... }
// export let VERSION = "1.0"
// export { calculate, VERSION }
// export { calculate as calc }
struct ExportDeclaration {
    std::vector<ExportedSymbol> exports;    // What to export
    bool isDefaultExport;                   // export default
    std::unique_ptr<Statement> declaration; // The declaration being exported
    
    ExportDeclaration() : isDefaultExport(false) {}
};

// Dependency graph for detecting circular imports
class DependencyGraph {
private:
    std::map<std::string, std::set<std::string>> adjacencyList;
    
public:
    void addDependency(const std::string& from, const std::string& to) {
        adjacencyList[from].insert(to);
    }
    
    bool hasCircularDependency(const std::string& start) {
        std::set<std::string> visited;
        std::set<std::string> recursionStack;
        return detectCycleDFS(start, visited, recursionStack);
    }
    
    std::vector<std::string> getTopologicalOrder() {
        std::set<std::string> visited;
        std::vector<std::string> result;
        
        for (const auto& pair : adjacencyList) {
            if (visited.find(pair.first) == visited.end()) {
                topologicalSortDFS(pair.first, visited, result);
            }
        }
        
        // Reverse for correct order
        std::reverse(result.begin(), result.end());
        return result;
    }
    
private:
    bool detectCycleDFS(const std::string& node, 
                        std::set<std::string>& visited,
                        std::set<std::string>& recursionStack) {
        visited.insert(node);
        recursionStack.insert(node);
        
        auto it = adjacencyList.find(node);
        if (it != adjacencyList.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    if (detectCycleDFS(neighbor, visited, recursionStack)) {
                        return true;
                    }
                } else if (recursionStack.find(neighbor) != recursionStack.end()) {
                    return true;  // Cycle detected
                }
            }
        }
        
        recursionStack.erase(node);
        return false;
    }
    
    void topologicalSortDFS(const std::string& node,
                            std::set<std::string>& visited,
                            std::vector<std::string>& result) {
        visited.insert(node);
        
        auto it = adjacencyList.find(node);
        if (it != adjacencyList.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    topologicalSortDFS(neighbor, visited, result);
                }
            }
        }
        
        result.push_back(node);
    }
};
