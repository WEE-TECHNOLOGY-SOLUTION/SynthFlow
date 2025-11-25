#include "../../include/ast.h"
#include <iostream>

void IntegerLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void FloatLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void StringLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BooleanLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void Identifier::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ArrayLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void IndexExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void AssignmentExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BinaryOp::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ExpressionStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void VariableDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void FunctionDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BlockStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void IfStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void WhileStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ForStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void BreakStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ContinueStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

void ReturnStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}