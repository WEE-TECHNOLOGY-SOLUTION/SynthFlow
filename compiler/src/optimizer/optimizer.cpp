#include "../../include/optimizer.h"
#include <iostream>
#include <algorithm>

// Constant Folding: Pre-compute constant expressions at compile time
std::unique_ptr<Expression> Optimizer::foldBinaryExpression(BinaryExpression* node) {
    // Check if both operands are literals
    auto* leftInt = dynamic_cast<IntegerLiteral*>(node->left.get());
    auto* rightInt = dynamic_cast<IntegerLiteral*>(node->right.get());
    auto* leftFloat = dynamic_cast<FloatLiteral*>(node->left.get());
    auto* rightFloat = dynamic_cast<FloatLiteral*>(node->right.get());
    auto* leftBool = dynamic_cast<BooleanLiteral*>(node->left.get());
    auto* rightBool = dynamic_cast<BooleanLiteral*>(node->right.get());
    
    // Integer + Integer folding
    if (leftInt && rightInt) {
        int64_t left = leftInt->value;
        int64_t right = rightInt->value;
        int64_t result = 0;
        
        if (node->op == "+") result = left + right;
        else if (node->op == "-") result = left - right;
        else if (node->op == "*") result = left * right;
        else if (node->op == "/" && right != 0) result = left / right;
        else if (node->op == "%") result = left % right;
        else return nullptr;
        
        return std::make_unique<IntegerLiteral>(result);
    }
    
    // Float folding
    if ((leftFloat || leftInt) && (rightFloat || rightInt)) {
        double left = leftFloat ? leftFloat->value : static_cast<double>(leftInt->value);
        double right = rightFloat ? rightFloat->value : static_cast<double>(rightInt->value);
        
        if (node->op == "+") return std::make_unique<FloatLiteral>(left + right);
        if (node->op == "-") return std::make_unique<FloatLiteral>(left - right);
        if (node->op == "*") return std::make_unique<FloatLiteral>(left * right);
        if (node->op == "/" && right != 0) return std::make_unique<FloatLiteral>(left / right);
    }
    
    // Boolean folding
    if (leftBool && rightBool) {
        bool left = leftBool->value;
        bool right = rightBool->value;
        
        if (node->op == "&&" || node->op == "and") 
            return std::make_unique<BooleanLiteral>(left && right);
        if (node->op == "||" || node->op == "or") 
            return std::make_unique<BooleanLiteral>(left || right);
    }
    
    // Comparison folding for integers
    if (leftInt && rightInt) {
        int64_t left = leftInt->value;
        int64_t right = rightInt->value;
        
        if (node->op == "==") return std::make_unique<BooleanLiteral>(left == right);
        if (node->op == "!=") return std::make_unique<BooleanLiteral>(left != right);
        if (node->op == "<") return std::make_unique<BooleanLiteral>(left < right);
        if (node->op == ">") return std::make_unique<BooleanLiteral>(left > right);
        if (node->op == "<=") return std::make_unique<BooleanLiteral>(left <= right);
        if (node->op == ">=") return std::make_unique<BooleanLiteral>(left >= right);
    }
    
    return nullptr; // Cannot fold
}

std::unique_ptr<Expression> Optimizer::foldUnaryExpression(UnaryExpression* node) {
    auto* intLit = dynamic_cast<IntegerLiteral*>(node->operand.get());
    auto* floatLit = dynamic_cast<FloatLiteral*>(node->operand.get());
    auto* boolLit = dynamic_cast<BooleanLiteral*>(node->operand.get());
    
    if (node->op == "-" && intLit) {
        return std::make_unique<IntegerLiteral>(-intLit->value);
    }
    if (node->op == "-" && floatLit) {
        return std::make_unique<FloatLiteral>(-floatLit->value);
    }
    if ((node->op == "!" || node->op == "not") && boolLit) {
        return std::make_unique<BooleanLiteral>(!boolLit->value);
    }
    
    return nullptr;
}

void Optimizer::collectUsedVariables(Expression* expr) {
    if (!expr) return;
    
    if (auto* id = dynamic_cast<Identifier*>(expr)) {
        usedVariables.insert(id->name);
    } else if (auto* binary = dynamic_cast<BinaryExpression*>(expr)) {
        collectUsedVariables(binary->left.get());
        collectUsedVariables(binary->right.get());
    } else if (auto* unary = dynamic_cast<UnaryExpression*>(expr)) {
        collectUsedVariables(unary->operand.get());
    } else if (auto* call = dynamic_cast<CallExpression*>(expr)) {
        usedVariables.insert(call->callee);
        for (auto& arg : call->arguments) {
            collectUsedVariables(arg.get());
        }
    } else if (auto* assign = dynamic_cast<AssignmentExpression*>(expr)) {
        collectUsedVariables(assign->left.get());
        collectUsedVariables(assign->right.get());
    } else if (auto* arrIdx = dynamic_cast<ArrayIndexExpression*>(expr)) {
        collectUsedVariables(arrIdx->array.get());
        collectUsedVariables(arrIdx->index.get());
    }
}

bool Optimizer::isVariableUsed(const std::string& name) const {
    return usedVariables.find(name) != usedVariables.end();
}

std::unique_ptr<Expression> Optimizer::optimizeExpression(std::unique_ptr<Expression> expr) {
    if (!expr) return expr;
    
    // Try constant folding on binary expressions
    if (auto* binary = dynamic_cast<BinaryExpression*>(expr.get())) {
        // First optimize children
        binary->left = optimizeExpression(std::move(binary->left));
        binary->right = optimizeExpression(std::move(binary->right));
        
        // Then try to fold
        auto folded = foldBinaryExpression(binary);
        if (folded) return folded;
    }
    
    // Try constant folding on unary expressions
    if (auto* unary = dynamic_cast<UnaryExpression*>(expr.get())) {
        unary->operand = optimizeExpression(std::move(unary->operand));
        
        auto folded = foldUnaryExpression(unary);
        if (folded) return folded;
    }
    
    return expr;
}

std::unique_ptr<Statement> Optimizer::optimizeStatement(std::unique_ptr<Statement> stmt) {
    if (!stmt) return stmt;
    
    // Optimize variable declaration initializer
    if (auto* varDecl = dynamic_cast<VariableDeclaration*>(stmt.get())) {
        if (varDecl->initializer) {
            varDecl->initializer = optimizeExpression(std::move(varDecl->initializer));
        }
    }
    
    // Optimize expression statement
    if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt.get())) {
        exprStmt->expression = optimizeExpression(std::move(exprStmt->expression));
    }
    
    // Optimize if statement condition and branches
    if (auto* ifStmt = dynamic_cast<IfStatement*>(stmt.get())) {
        ifStmt->condition = optimizeExpression(std::move(ifStmt->condition));
        
        // Constant condition elimination
        if (auto* boolLit = dynamic_cast<BooleanLiteral*>(ifStmt->condition.get())) {
            if (boolLit->value) {
                // if (true) - keep only then branch
                return std::move(ifStmt->thenBranch);
            } else {
                // if (false) - keep only else branch or nothing
                if (ifStmt->elseBranch) {
                    return std::move(ifStmt->elseBranch);
                }
                return nullptr; // Remove entire if statement
            }
        }
    }
    
    // Optimize while statement
    if (auto* whileStmt = dynamic_cast<WhileStatement*>(stmt.get())) {
        whileStmt->condition = optimizeExpression(std::move(whileStmt->condition));
        
        // while (false) can be eliminated
        if (auto* boolLit = dynamic_cast<BooleanLiteral*>(whileStmt->condition.get())) {
            if (!boolLit->value) {
                return nullptr; // Remove unreachable loop
            }
        }
    }
    
    return stmt;
}

void Optimizer::constantFoldingPass(std::vector<std::unique_ptr<Statement>>& statements) {
    for (auto& stmt : statements) {
        stmt = optimizeStatement(std::move(stmt));
    }
    
    // Remove null statements
    statements.erase(
        std::remove_if(statements.begin(), statements.end(),
            [](const std::unique_ptr<Statement>& s) { return s == nullptr; }),
        statements.end()
    );
}

void Optimizer::deadCodeEliminationPass(std::vector<std::unique_ptr<Statement>>& statements) {
    // First pass: collect all used variables
    usedVariables.clear();
    
    for (auto& stmt : statements) {
        if (auto* varDecl = dynamic_cast<VariableDeclaration*>(stmt.get())) {
            if (varDecl->initializer) {
                collectUsedVariables(varDecl->initializer.get());
            }
        } else if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt.get())) {
            collectUsedVariables(exprStmt->expression.get());
        } else if (auto* ifStmt = dynamic_cast<IfStatement*>(stmt.get())) {
            collectUsedVariables(ifStmt->condition.get());
        } else if (auto* whileStmt = dynamic_cast<WhileStatement*>(stmt.get())) {
            collectUsedVariables(whileStmt->condition.get());
        }
    }
    
    // Second pass: remove unused variable declarations (keep side effects)
    // For now, we only remove truly unused pure declarations
    // This is conservative - we keep anything that might have side effects
}

void Optimizer::optimize(std::vector<std::unique_ptr<Statement>>& statements) {
    // Run constant folding
    constantFoldingPass(statements);
    
    // Run dead code elimination
    deadCodeEliminationPass(statements);
}
