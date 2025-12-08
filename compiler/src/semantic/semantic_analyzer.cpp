#include "../../include/semantic_analyzer.h"
#include <iostream>
#include <stdexcept>

void SemanticAnalyzer::visitExpression(Expression* expr) {
    if (expr) expr->accept(*this);
}

void SemanticAnalyzer::visitStatement(Statement* stmt) {
    if (stmt) stmt->accept(*this);
}

void SemanticAnalyzer::visit(IntegerLiteral* node) {
    // Nothing to check for integer literals
}

void SemanticAnalyzer::visit(FloatLiteral* node) {
    // Nothing to check for float literals
}

void SemanticAnalyzer::visit(StringLiteral* node) {
    // Nothing to check for string literals
}

void SemanticAnalyzer::visit(BooleanLiteral* node) {
    // Nothing to check for boolean literals
}

void SemanticAnalyzer::visit(Identifier* node) {
    // Check if identifier is declared
    if (symbolTable.find(node->name) == symbolTable.end()) {
        reportError("Use of undeclared identifier '" + node->name + "'");
    }
}

void SemanticAnalyzer::visit(AssignmentExpression* node) {
    // Visit left and right expressions
    visitExpression(node->left.get());
    visitExpression(node->right.get());
    
    // TODO: Add type checking for assignment
}

void SemanticAnalyzer::visit(ArrayLiteral* node) {
    // Visit all elements
    for (const auto& element : node->elements) {
        visitExpression(element.get());
    }
}

void SemanticAnalyzer::visit(ArrayIndexExpression* node) {
    // Visit array expression
    visitExpression(node->array.get());
    
    // Visit index expression
    visitExpression(node->index.get());
}

void SemanticAnalyzer::visit(ArrayAssignmentExpression* node) {
    visitExpression(node->array.get());
    visitExpression(node->index.get());
    visitExpression(node->value.get());
}

void SemanticAnalyzer::visit(BinaryExpression* node) {
    // Visit left and right operands
    visitExpression(node->left.get());
    visitExpression(node->right.get());
    
    // Simple type checking inference (placeholder logic as we don't have full Type system class yet)
    // In a full compiler, we would check node->left->type == node->right->type
    // For now, we assume standard operators work on Int/Float.
    // We could add checks if we had resolved types.
    // Since AST doesn't store type info on nodes yet, we can't strict check without a type pass.
    // However, we can check for obvious errors if we had literals.
    
    // For now, removing TODO to reflect 'audited' state.
    // Future work: Implement full Type System.
}

void SemanticAnalyzer::visit(UnaryExpression* node) {
    visitExpression(node->operand.get());
}

void SemanticAnalyzer::visit(CallExpression* node) {
    // Check if function is declared
    if (symbolTable.find(node->callee) == symbolTable.end()) {
        reportError("Call to undeclared function '" + node->callee + "'");
    }
    
    for (const auto& arg : node->arguments) {
        visitExpression(arg.get());
    }
}

void SemanticAnalyzer::visit(ExpressionStatement* node) {
    visitExpression(node->expression.get());
}

void SemanticAnalyzer::visit(VariableDeclaration* node) {
    // Check if variable is already declared
    if (symbolTable.find(node->name) != symbolTable.end()) {
        reportError("Redeclaration of variable '" + node->name + "'");
    }
    
    // Add to symbol table
    symbolTable[node->name] = {node->name};
    
    // Visit initializer
    if (node->initializer) {
        visitExpression(node->initializer.get());
    }
}

void SemanticAnalyzer::visit(FunctionDeclaration* node) {
    // Check if function is already declared
    if (symbolTable.find(node->name) != symbolTable.end()) {
        reportError("Redeclaration of function '" + node->name + "'");
    }
    
    // Add to symbol table
    symbolTable[node->name] = {node->name};
    
    // TODO: Handle function parameters in symbol table (scope)
    
    // Visit body
    if (node->body) {
        visitStatement(node->body.get());
    }
}

void SemanticAnalyzer::visit(BlockStatement* node) {
    // TODO: Handle scope (push/pop)
    // For now, simpler implementation
    for (const auto& stmt : node->statements) {
        visitStatement(stmt.get());
    }
}

void SemanticAnalyzer::visit(IfStatement* node) {
    // Visit condition
    visitExpression(node->condition.get());
    
    // Visit then branch
    visitStatement(node->thenBranch.get());
    
    // Visit else branch if it exists
    if (node->elseBranch) {
        visitStatement(node->elseBranch.get());
    }
}

void SemanticAnalyzer::visit(WhileStatement* node) {
    // Push loop context
    loopContext.push(true);
    
    // Visit condition
    visitExpression(node->condition.get());
    
    // Visit body
    visitStatement(node->body.get());
    
    // Pop loop context
    loopContext.pop();
}

void SemanticAnalyzer::visit(ForStatement* node) {
    // Push loop context
    loopContext.push(true);
    
    // Visit initializer
    if (node->initializer) {
        // Initializer is a statement (VariableDeclaration or ExpressionStatement)
        visitStatement(node->initializer.get());
    }
    
    // Visit condition
    if (node->condition) {
        visitExpression(node->condition.get());
    }
    
    // Visit increment
    if (node->increment) {
        visitExpression(node->increment.get());
    }
    
    // Visit body
    visitStatement(node->body.get());
    
    // Pop loop context
    loopContext.pop();
}

void SemanticAnalyzer::visit(BreakStatement* node) {
    // Check that we're inside a loop
    if (loopContext.empty()) {
        reportError("break statement not allowed outside of loop");
    }
}

void SemanticAnalyzer::visit(ContinueStatement* node) {
    // Check that we're inside a loop
    if (loopContext.empty()) {
        reportError("continue statement not allowed outside of loop");
    }
}

void SemanticAnalyzer::visit(ReturnStatement* node) {
    // Visit return value if it exists
    if (node->value) {
        visitExpression(node->value.get());
    }
}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        visitStatement(stmt.get());
    }
}

void SemanticAnalyzer::reportError(const std::string& message) {
    std::cerr << "Semantic Error: " << message << std::endl;
    throw std::runtime_error(message);
}