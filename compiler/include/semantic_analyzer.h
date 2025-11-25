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
        // TODO: Add type information
    };
    
    std::unordered_map<std::string, Symbol> symbolTable;
    std::stack<bool> loopContext; // Track if we're inside a loop
    
    void visitExpression(Expression* expr);
    void visitStatement(Statement* stmt);
    
    void visit(IntegerLiteral* node);
    void visit(FloatLiteral* node);
    void visit(StringLiteral* node);
    void visit(BooleanLiteral* node);
    void visit(Identifier* node);
    void visit(ArrayLiteral* node);
    void visit(IndexExpression* node);
    void visit(AssignmentExpression* node);
    void visit(BinaryOp* node);
    
    void visit(ExpressionStatement* node);
    void visit(VariableDeclaration* node);
    void visit(FunctionDeclaration* node);
    void visit(BlockStatement* node);
    void visit(IfStatement* node);
    void visit(WhileStatement* node);
    void visit(ForStatement* node);
    void visit(BreakStatement* node);
    void visit(ContinueStatement* node);
    void visit(ReturnStatement* node);
    
public:
    SemanticAnalyzer() = default;
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Error reporting
    void reportError(const std::string& message);
};