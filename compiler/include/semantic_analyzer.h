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
    }
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Error reporting
    void reportError(const std::string& message);
};