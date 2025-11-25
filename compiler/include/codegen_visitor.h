#pragma once
#include "ast.h"
#include <string>

class CodegenVisitor {
public:
    virtual ~CodegenVisitor() = default;
    
    // Expression visitors that return values
    virtual std::string visitExpr(IntegerLiteral* node) = 0;
    virtual std::string visitExpr(FloatLiteral* node) = 0;
    virtual std::string visitExpr(StringLiteral* node) = 0;
    virtual std::string visitExpr(BooleanLiteral* node) = 0;
    virtual std::string visitExpr(Identifier* node) = 0;
    virtual std::string visitExpr(BinaryOp* node) = 0;
    
    // Statement visitors that don't return values
    virtual void visitStmt(ExpressionStatement* node) = 0;
    virtual void visitStmt(VariableDeclaration* node) = 0;
    virtual void visitStmt(FunctionDeclaration* node) = 0;
    virtual void visitStmt(BlockStatement* node) = 0;
    virtual void visitStmt(IfStatement* node) = 0;
    virtual void visitStmt(ReturnStatement* node) = 0;
};