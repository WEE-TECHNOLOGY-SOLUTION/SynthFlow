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
        // TODO: Add type information
    };
    
    std::unordered_map<std::string, Symbol> symbolTable;
    std::stack<bool> loopContext; // Track if we're inside a loop
    
    void visitExpression(Expression* expr);
    void visitStatement(Statement* stmt);
    
    void visit(IntegerLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(AssignmentExpression* node) override;
    void visit(CallExpression* node) override;
    void visit(ArrayLiteral* node) override;
    void visit(ArrayIndexExpression* node) override;
    void visit(ArrayAssignmentExpression* node) override;
    
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
    
public:
    SemanticAnalyzer() {
        // Register built-in functions
        symbolTable["print"] = {"print", true};
        symbolTable["input"] = {"input", true};
        symbolTable["len"] = {"len", true};
        symbolTable["str"] = {"str", true};
        symbolTable["int"] = {"int", true};
        symbolTable["float"] = {"float", true};
        symbolTable["read_file"] = {"read_file", true};
        symbolTable["write_file"] = {"write_file", true};
    }
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Error reporting
    void reportError(const std::string& message);
};