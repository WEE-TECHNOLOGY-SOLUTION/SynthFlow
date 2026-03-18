#pragma once
#include "ast.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <stack>

class SemanticAnalyzer : public ASTVisitor {
private:
    struct Symbol {
        std::string name;
        bool isBuiltin = false;
        bool isConst = false;    // Track if variable is constant
        std::string typeName = ""; // Track type annotation
        bool isNullable = false;   // Track if type is nullable
    };

    // Scope stack: each scope is a map of name -> Symbol
    // The back of the vector is the current (innermost) scope
    std::vector<std::unordered_map<std::string, Symbol>> scopeStack;
    std::stack<bool> loopContext; // Track if we're inside a loop

    // Push a new scope
    void pushScope() {
        scopeStack.push_back(std::unordered_map<std::string, Symbol>());
    }

    // Pop the current scope
    void popScope() {
        if (scopeStack.size() > 1) {  // Never pop the global scope
            scopeStack.pop_back();
        }
    }

    // Declare a symbol in the current scope
    void declareSymbol(const std::string& name, const Symbol& symbol) {
        scopeStack.back()[name] = symbol;
    }

    // Look up a symbol, starting from current scope and going outward
    Symbol* lookupSymbol(const std::string& name) {
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            auto symIt = it->find(name);
            if (symIt != it->end()) {
                return &symIt->second;
            }
        }
        return nullptr;
    }

    // Check if symbol exists in current scope only
    bool symbolExistsInCurrentScope(const std::string& name) {
        return scopeStack.back().find(name) != scopeStack.back().end();
    }
    
    void visitExpression(Expression* expr);
    void visitStatement(Statement* stmt);
    
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
    void visit(LambdaExpression* node) override;
    void visit(MatchExpression* node) override;
    void visit(CompoundAssignment* node) override;
    void visit(UpdateExpression* node) override;
    void visit(InterpolatedString* node) override;
    
    // SADK Expression visitors (Agent Development Kit)
    void visit(MapLiteral* node) override;
    void visit(MemberExpression* node) override;
    void visit(MethodCallExpression* node) override;
    void visit(SelfExpression* node) override;
    
    void visit(ExpressionStatement* node) override;
    void visit(VariableDeclaration* node) override;
    void visit(FunctionDeclaration* node) override;
    void visit(BlockStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ForStatement* node) override;
    void visit(BreakStatement* node) override;
    void visit(ContinueStatement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(TryStatement* node) override;
    
    // SADK Statement visitors (Agent Development Kit)
    void visit(ImportStatement* node) override;
    void visit(StructDeclaration* node) override;
    
public:
    SemanticAnalyzer() {
        // Create global scope
        pushScope();

        // Register built-in functions in global scope
        scopeStack.back()["print"] = {"print", true, false, "", false};
        scopeStack.back()["input"] = {"input", true, false, "", false};
        scopeStack.back()["len"] = {"len", true, false, "", false};
        scopeStack.back()["str"] = {"str", true, false, "", false};
        scopeStack.back()["int"] = {"int", true, false, "", false};
        scopeStack.back()["float"] = {"float", true, false, "", false};
        scopeStack.back()["read_file"] = {"read_file", true, false, "", false};
        scopeStack.back()["write_file"] = {"write_file", true, false, "", false};

        // Gemini API built-in functions
        scopeStack.back()["gemini_set_api_key"] = {"gemini_set_api_key", true, false, "", false};
        scopeStack.back()["gemini_has_api_key"] = {"gemini_has_api_key", true, false, "", false};
        scopeStack.back()["gemini_complete"] = {"gemini_complete", true, false, "", false};
        scopeStack.back()["gemini_chat"] = {"gemini_chat", true, false, "", false};

        // HTTP built-in functions
        scopeStack.back()["http_get"] = {"http_get", true, false, "", false};
        scopeStack.back()["http_post"] = {"http_post", true, false, "", false};

        // Array built-in functions
        scopeStack.back()["append"] = {"append", true, false, "", false};
        scopeStack.back()["push"] = {"push", true, false, "", false};
        scopeStack.back()["pop"] = {"pop", true, false, "", false};
        scopeStack.back()["slice"] = {"slice", true, false, "", false};
        scopeStack.back()["shift"] = {"shift", true, false, "", false};
        scopeStack.back()["unshift"] = {"unshift", true, false, "", false};
        scopeStack.back()["indexOf"] = {"indexOf", true, false, "", false};
        scopeStack.back()["contains"] = {"contains", true, false, "", false};
        scopeStack.back()["typeof"] = {"typeof", true, false, "", false};
        scopeStack.back()["range"] = {"range", true, false, "", false};

        // Math built-in functions
        scopeStack.back()["abs"] = {"abs", true, false, "", false};
        scopeStack.back()["sqrt"] = {"sqrt", true, false, "", false};
        scopeStack.back()["pow"] = {"pow", true, false, "", false};
        scopeStack.back()["sin"] = {"sin", true, false, "", false};
        scopeStack.back()["cos"] = {"cos", true, false, "", false};
        scopeStack.back()["exp"] = {"exp", true, false, "", false};
        scopeStack.back()["ln"] = {"ln", true, false, "", false};
        scopeStack.back()["floor"] = {"floor", true, false, "", false};
        scopeStack.back()["ceil"] = {"ceil", true, false, "", false};
        scopeStack.back()["round"] = {"round", true, false, "", false};

        // Web Framework built-in functions
        scopeStack.back()["route"] = {"route", true, false, "", false};
        scopeStack.back()["serve"] = {"serve", true, false, "", false};
        scopeStack.back()["json"] = {"json", true, false, "", false};
        scopeStack.back()["html"] = {"html", true, false, "", false};
        scopeStack.back()["text"] = {"text", true, false, "", false};
        scopeStack.back()["use"] = {"use", true, false, "", false};

        // OS & Subprocess built-in functions (Python-like)
        scopeStack.back()["__builtin_exec"] = {"__builtin_exec", true, false, "", false};
        scopeStack.back()["__builtin_shell"] = {"__builtin_shell", true, false, "", false};
        scopeStack.back()["__builtin_env_get"] = {"__builtin_env_get", true, false, "", false};
        scopeStack.back()["__builtin_env_set"] = {"__builtin_env_set", true, false, "", false};
        scopeStack.back()["__builtin_getcwd"] = {"__builtin_getcwd", true, false, "", false};
        scopeStack.back()["__builtin_chdir"] = {"__builtin_chdir", true, false, "", false};
        scopeStack.back()["__builtin_platform"] = {"__builtin_platform", true, false, "", false};
        scopeStack.back()["__builtin_arch"] = {"__builtin_arch", true, false, "", false};
        scopeStack.back()["__builtin_hostname"] = {"__builtin_hostname", true, false, "", false};
        scopeStack.back()["__builtin_username"] = {"__builtin_username", true, false, "", false};
        scopeStack.back()["__builtin_homedir"] = {"__builtin_homedir", true, false, "", false};
        scopeStack.back()["__builtin_tempdir"] = {"__builtin_tempdir", true, false, "", false};
        scopeStack.back()["__builtin_path_exists"] = {"__builtin_path_exists", true, false, "", false};
        scopeStack.back()["__builtin_is_file"] = {"__builtin_is_file", true, false, "", false};
        scopeStack.back()["__builtin_is_dir"] = {"__builtin_is_dir", true, false, "", false};
        scopeStack.back()["__builtin_listdir"] = {"__builtin_listdir", true, false, "", false};
        scopeStack.back()["__builtin_mkdir"] = {"__builtin_mkdir", true, false, "", false};
        scopeStack.back()["__builtin_rmdir"] = {"__builtin_rmdir", true, false, "", false};
        scopeStack.back()["__builtin_remove"] = {"__builtin_remove", true, false, "", false};
        scopeStack.back()["__builtin_rename"] = {"__builtin_rename", true, false, "", false};
        scopeStack.back()["__builtin_getpid"] = {"__builtin_getpid", true, false, "", false};
        scopeStack.back()["__builtin_exit"] = {"__builtin_exit", true, false, "", false};
        scopeStack.back()["__builtin_time"] = {"__builtin_time", true, false, "", false};
        scopeStack.back()["__builtin_time_ms"] = {"__builtin_time_ms", true, false, "", false};
        scopeStack.back()["__builtin_sleep"] = {"__builtin_sleep", true, false, "", false};
        scopeStack.back()["__builtin_substring"] = {"__builtin_substring", true, false, "", false};
        scopeStack.back()["__builtin_which"] = {"__builtin_which", true, false, "", false};

        // Networking built-in functions
        scopeStack.back()["__builtin_tcp_connect"] = {"__builtin_tcp_connect", true, false, "", false};
        scopeStack.back()["__builtin_tcp_send"] = {"__builtin_tcp_send", true, false, "", false};
        scopeStack.back()["__builtin_tcp_recv"] = {"__builtin_tcp_recv", true, false, "", false};
        scopeStack.back()["__builtin_tcp_close"] = {"__builtin_tcp_close", true, false, "", false};
        scopeStack.back()["__builtin_tcp_listen"] = {"__builtin_tcp_listen", true, false, "", false};
        scopeStack.back()["__builtin_tcp_accept"] = {"__builtin_tcp_accept", true, false, "", false};
        scopeStack.back()["__builtin_dns_lookup"] = {"__builtin_dns_lookup", true, false, "", false};
        scopeStack.back()["__builtin_port_check"] = {"__builtin_port_check", true, false, "", false};
        scopeStack.back()["__builtin_get_local_ip"] = {"__builtin_get_local_ip", true, false, "", false};
        scopeStack.back()["__builtin_udp_create"] = {"__builtin_udp_create", true, false, "", false};
        scopeStack.back()["__builtin_udp_sendto"] = {"__builtin_udp_sendto", true, false, "", false};
        scopeStack.back()["__builtin_udp_close"] = {"__builtin_udp_close", true, false, "", false};

        // Security built-in functions
        scopeStack.back()["__builtin_base64url_encode"] = {"__builtin_base64url_encode", true, false, "", false};
        scopeStack.back()["__builtin_base64url_decode"] = {"__builtin_base64url_decode", true, false, "", false};
        scopeStack.back()["__builtin_regex_test"] = {"__builtin_regex_test", true, false, "", false};
        scopeStack.back()["__builtin_split"] = {"__builtin_split", true, false, "", false};
        scopeStack.back()["__builtin_join"] = {"__builtin_join", true, false, "", false};
        scopeStack.back()["__builtin_trim"] = {"__builtin_trim", true, false, "", false};
        scopeStack.back()["__builtin_lowercase"] = {"__builtin_lowercase", true, false, "", false};
        scopeStack.back()["__builtin_starts_with"] = {"__builtin_starts_with", true, false, "", false};
        scopeStack.back()["__builtin_contains"] = {"__builtin_contains", true, false, "", false};
        scopeStack.back()["__builtin_replace_all"] = {"__builtin_replace_all", true, false, "", false};
        scopeStack.back()["__builtin_json_stringify"] = {"__builtin_json_stringify", true, false, "", false};
        scopeStack.back()["__builtin_json_parse"] = {"__builtin_json_parse", true, false, "", false};
        scopeStack.back()["__builtin_random_bytes"] = {"__builtin_random_bytes", true, false, "", false};
        scopeStack.back()["__builtin_uuid"] = {"__builtin_uuid", true, false, "", false};
        scopeStack.back()["__builtin_secure_compare"] = {"__builtin_secure_compare", true, false, "", false};

        // API management built-in functions
        scopeStack.back()["__builtin_keys"] = {"__builtin_keys", true, false, "", false};
        scopeStack.back()["__builtin_index_of"] = {"__builtin_index_of", true, false, "", false};
        scopeStack.back()["__builtin_uppercase"] = {"__builtin_uppercase", true, false, "", false};
        scopeStack.back()["__builtin_ends_with"] = {"__builtin_ends_with", true, false, "", false};

        // FFI / Systems built-in functions
        scopeStack.back()["__builtin_alloc_buffer"] = {"__builtin_alloc_buffer", true, false, "", false};
        scopeStack.back()["__builtin_free_buffer"] = {"__builtin_free_buffer", true, false, "", false};
        scopeStack.back()["__builtin_buffer_write"] = {"__builtin_buffer_write", true, false, "", false};
        scopeStack.back()["__builtin_buffer_read"] = {"__builtin_buffer_read", true, false, "", false};
        scopeStack.back()["__builtin_load_library"] = {"__builtin_load_library", true, false, "", false};
        scopeStack.back()["__builtin_unload_library"] = {"__builtin_unload_library", true, false, "", false};
        scopeStack.back()["__builtin_get_proc_address"] = {"__builtin_get_proc_address", true, false, "", false};
        scopeStack.back()["__builtin_ffi_call"] = {"__builtin_ffi_call", true, false, "", false};
        scopeStack.back()["__builtin_mmap"] = {"__builtin_mmap", true, false, "", false};
        scopeStack.back()["__builtin_munmap"] = {"__builtin_munmap", true, false, "", false};
        scopeStack.back()["__builtin_gpio_mode"] = {"__builtin_gpio_mode", true, false, "", false};
        scopeStack.back()["__builtin_gpio_write"] = {"__builtin_gpio_write", true, false, "", false};
        scopeStack.back()["__builtin_gpio_read"] = {"__builtin_gpio_read", true, false, "", false};
        scopeStack.back()["__builtin_i2c_open"] = {"__builtin_i2c_open", true, false, "", false};
        scopeStack.back()["__builtin_i2c_write"] = {"__builtin_i2c_write", true, false, "", false};
        scopeStack.back()["__builtin_i2c_read"] = {"__builtin_i2c_read", true, false, "", false};
        scopeStack.back()["__builtin_i2c_close"] = {"__builtin_i2c_close", true, false, "", false};
        scopeStack.back()["__builtin_spi_open"] = {"__builtin_spi_open", true, false, "", false};
        scopeStack.back()["__builtin_spi_transfer"] = {"__builtin_spi_transfer", true, false, "", false};
        scopeStack.back()["__builtin_spi_close"] = {"__builtin_spi_close", true, false, "", false};
    }
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Error reporting
    void reportError(const std::string& message);
};