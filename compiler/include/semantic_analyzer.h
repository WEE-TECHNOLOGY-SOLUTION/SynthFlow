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
    
    std::unordered_map<std::string, Symbol> symbolTable;
    std::stack<bool> loopContext; // Track if we're inside a loop
    
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
        // Register built-in functions
        symbolTable["print"] = {"print", true, false, "", false};
        symbolTable["input"] = {"input", true, false, "", false};
        symbolTable["len"] = {"len", true, false, "", false};
        symbolTable["str"] = {"str", true, false, "", false};
        symbolTable["int"] = {"int", true, false, "", false};
        symbolTable["float"] = {"float", true, false, "", false};
        symbolTable["read_file"] = {"read_file", true, false, "", false};
        symbolTable["write_file"] = {"write_file", true, false, "", false};
        
        // Gemini API built-in functions
        symbolTable["gemini_set_api_key"] = {"gemini_set_api_key", true, false, "", false};
        symbolTable["gemini_has_api_key"] = {"gemini_has_api_key", true, false, "", false};
        symbolTable["gemini_complete"] = {"gemini_complete", true, false, "", false};
        symbolTable["gemini_chat"] = {"gemini_chat", true, false, "", false};
        
        // HTTP built-in functions
        symbolTable["http_get"] = {"http_get", true, false, "", false};
        symbolTable["http_post"] = {"http_post", true, false, "", false};
        
        // Array built-in functions
        symbolTable["append"] = {"append", true, false, "", false};
        symbolTable["push"] = {"push", true, false, "", false};
        symbolTable["typeof"] = {"typeof", true, false, "", false};
        symbolTable["range"] = {"range", true, false, "", false};
        
        // Math built-in functions
        symbolTable["abs"] = {"abs", true, false, "", false};
        symbolTable["sqrt"] = {"sqrt", true, false, "", false};
        symbolTable["pow"] = {"pow", true, false, "", false};
        symbolTable["sin"] = {"sin", true, false, "", false};
        symbolTable["cos"] = {"cos", true, false, "", false};
        symbolTable["exp"] = {"exp", true, false, "", false};
        symbolTable["ln"] = {"ln", true, false, "", false};
        symbolTable["floor"] = {"floor", true, false, "", false};
        symbolTable["ceil"] = {"ceil", true, false, "", false};
        symbolTable["round"] = {"round", true, false, "", false};
        
        // Web Framework built-in functions
        symbolTable["route"] = {"route", true, false, "", false};
        symbolTable["serve"] = {"serve", true, false, "", false};
        symbolTable["json"] = {"json", true, false, "", false};
        symbolTable["html"] = {"html", true, false, "", false};
        symbolTable["text"] = {"text", true, false, "", false};
        symbolTable["use"] = {"use", true, false, "", false};
        
        // OS & Subprocess built-in functions (Python-like)
        symbolTable["__builtin_exec"] = {"__builtin_exec", true, false, "", false};
        symbolTable["__builtin_shell"] = {"__builtin_shell", true, false, "", false};
        symbolTable["__builtin_env_get"] = {"__builtin_env_get", true, false, "", false};
        symbolTable["__builtin_env_set"] = {"__builtin_env_set", true, false, "", false};
        symbolTable["__builtin_getcwd"] = {"__builtin_getcwd", true, false, "", false};
        symbolTable["__builtin_chdir"] = {"__builtin_chdir", true, false, "", false};
        symbolTable["__builtin_platform"] = {"__builtin_platform", true, false, "", false};
        symbolTable["__builtin_arch"] = {"__builtin_arch", true, false, "", false};
        symbolTable["__builtin_hostname"] = {"__builtin_hostname", true, false, "", false};
        symbolTable["__builtin_username"] = {"__builtin_username", true, false, "", false};
        symbolTable["__builtin_homedir"] = {"__builtin_homedir", true, false, "", false};
        symbolTable["__builtin_tempdir"] = {"__builtin_tempdir", true, false, "", false};
        symbolTable["__builtin_path_exists"] = {"__builtin_path_exists", true, false, "", false};
        symbolTable["__builtin_is_file"] = {"__builtin_is_file", true, false, "", false};
        symbolTable["__builtin_is_dir"] = {"__builtin_is_dir", true, false, "", false};
        symbolTable["__builtin_listdir"] = {"__builtin_listdir", true, false, "", false};
        symbolTable["__builtin_mkdir"] = {"__builtin_mkdir", true, false, "", false};
        symbolTable["__builtin_rmdir"] = {"__builtin_rmdir", true, false, "", false};
        symbolTable["__builtin_remove"] = {"__builtin_remove", true, false, "", false};
        symbolTable["__builtin_rename"] = {"__builtin_rename", true, false, "", false};
        symbolTable["__builtin_getpid"] = {"__builtin_getpid", true, false, "", false};
        symbolTable["__builtin_exit"] = {"__builtin_exit", true, false, "", false};
        symbolTable["__builtin_time"] = {"__builtin_time", true, false, "", false};
        symbolTable["__builtin_time_ms"] = {"__builtin_time_ms", true, false, "", false};
        symbolTable["__builtin_sleep"] = {"__builtin_sleep", true, false, "", false};
        symbolTable["__builtin_substring"] = {"__builtin_substring", true, false, "", false};
        symbolTable["__builtin_which"] = {"__builtin_which", true, false, "", false};
        
        // Networking built-in functions
        symbolTable["__builtin_tcp_connect"] = {"__builtin_tcp_connect", true, false, "", false};
        symbolTable["__builtin_tcp_send"] = {"__builtin_tcp_send", true, false, "", false};
        symbolTable["__builtin_tcp_recv"] = {"__builtin_tcp_recv", true, false, "", false};
        symbolTable["__builtin_tcp_close"] = {"__builtin_tcp_close", true, false, "", false};
        symbolTable["__builtin_tcp_listen"] = {"__builtin_tcp_listen", true, false, "", false};
        symbolTable["__builtin_tcp_accept"] = {"__builtin_tcp_accept", true, false, "", false};
        symbolTable["__builtin_dns_lookup"] = {"__builtin_dns_lookup", true, false, "", false};
        symbolTable["__builtin_port_check"] = {"__builtin_port_check", true, false, "", false};
        symbolTable["__builtin_get_local_ip"] = {"__builtin_get_local_ip", true, false, "", false};
        symbolTable["__builtin_udp_create"] = {"__builtin_udp_create", true, false, "", false};
        symbolTable["__builtin_udp_sendto"] = {"__builtin_udp_sendto", true, false, "", false};
        symbolTable["__builtin_udp_close"] = {"__builtin_udp_close", true, false, "", false};
        
        // Security built-in functions
        symbolTable["__builtin_base64url_encode"] = {"__builtin_base64url_encode", true, false, "", false};
        symbolTable["__builtin_base64url_decode"] = {"__builtin_base64url_decode", true, false, "", false};
        symbolTable["__builtin_regex_test"] = {"__builtin_regex_test", true, false, "", false};
        symbolTable["__builtin_split"] = {"__builtin_split", true, false, "", false};
        symbolTable["__builtin_join"] = {"__builtin_join", true, false, "", false};
        symbolTable["__builtin_trim"] = {"__builtin_trim", true, false, "", false};
        symbolTable["__builtin_lowercase"] = {"__builtin_lowercase", true, false, "", false};
        symbolTable["__builtin_starts_with"] = {"__builtin_starts_with", true, false, "", false};
        symbolTable["__builtin_contains"] = {"__builtin_contains", true, false, "", false};
        symbolTable["__builtin_replace_all"] = {"__builtin_replace_all", true, false, "", false};
        symbolTable["__builtin_json_stringify"] = {"__builtin_json_stringify", true, false, "", false};
        symbolTable["__builtin_json_parse"] = {"__builtin_json_parse", true, false, "", false};
        symbolTable["__builtin_random_bytes"] = {"__builtin_random_bytes", true, false, "", false};
        symbolTable["__builtin_uuid"] = {"__builtin_uuid", true, false, "", false};
        symbolTable["__builtin_secure_compare"] = {"__builtin_secure_compare", true, false, "", false};
        
        // API management built-in functions
        symbolTable["__builtin_keys"] = {"__builtin_keys", true, false, "", false};
        symbolTable["__builtin_index_of"] = {"__builtin_index_of", true, false, "", false};
        symbolTable["__builtin_uppercase"] = {"__builtin_uppercase", true, false, "", false};
        symbolTable["__builtin_ends_with"] = {"__builtin_ends_with", true, false, "", false};
        
        // FFI / Systems built-in functions
        symbolTable["__builtin_alloc_buffer"] = {"__builtin_alloc_buffer", true, false, "", false};
        symbolTable["__builtin_free_buffer"] = {"__builtin_free_buffer", true, false, "", false};
        symbolTable["__builtin_buffer_write"] = {"__builtin_buffer_write", true, false, "", false};
        symbolTable["__builtin_buffer_read"] = {"__builtin_buffer_read", true, false, "", false};
        symbolTable["__builtin_time_ms"] = {"__builtin_time_ms", true, false, "", false};
        symbolTable["__builtin_sleep"] = {"__builtin_sleep", true, false, "", false};
        symbolTable["__builtin_substring"] = {"__builtin_substring", true, false, "", false};
        symbolTable["__builtin_load_library"] = {"__builtin_load_library", true, false, "", false};
        symbolTable["__builtin_unload_library"] = {"__builtin_unload_library", true, false, "", false};
        symbolTable["__builtin_get_proc_address"] = {"__builtin_get_proc_address", true, false, "", false};
        symbolTable["__builtin_ffi_call"] = {"__builtin_ffi_call", true, false, "", false};
        symbolTable["__builtin_mmap"] = {"__builtin_mmap", true, false, "", false};
        symbolTable["__builtin_munmap"] = {"__builtin_munmap", true, false, "", false};
        symbolTable["__builtin_gpio_mode"] = {"__builtin_gpio_mode", true, false, "", false};
        symbolTable["__builtin_gpio_write"] = {"__builtin_gpio_write", true, false, "", false};
        symbolTable["__builtin_gpio_read"] = {"__builtin_gpio_read", true, false, "", false};
        symbolTable["__builtin_i2c_open"] = {"__builtin_i2c_open", true, false, "", false};
        symbolTable["__builtin_i2c_write"] = {"__builtin_i2c_write", true, false, "", false};
        symbolTable["__builtin_i2c_read"] = {"__builtin_i2c_read", true, false, "", false};
        symbolTable["__builtin_i2c_close"] = {"__builtin_i2c_close", true, false, "", false};
        symbolTable["__builtin_spi_open"] = {"__builtin_spi_open", true, false, "", false};
        symbolTable["__builtin_spi_transfer"] = {"__builtin_spi_transfer", true, false, "", false};
        symbolTable["__builtin_spi_close"] = {"__builtin_spi_close", true, false, "", false};
    }
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Error reporting
    void reportError(const std::string& message);
};