#pragma once
#include "ast.h"
#include <memory>
#include <vector>
#include <unordered_set>

// AST Optimizer - performs compile-time optimizations
class Optimizer {
private:
    // Track used variables for dead code elimination
    std::unordered_set<std::string> usedVariables;
    
    // Constant folding helpers
    std::unique_ptr<Expression> foldBinaryExpression(BinaryExpression* node);
    std::unique_ptr<Expression> foldUnaryExpression(UnaryExpression* node);
    
    // Dead code elimination helpers
    void collectUsedVariables(Expression* expr);
    bool isVariableUsed(const std::string& name) const;
    
public:
    Optimizer() = default;
    
    // Main optimization passes
    void optimize(std::vector<std::unique_ptr<Statement>>& statements);
    
    // Individual optimizations
    std::unique_ptr<Expression> optimizeExpression(std::unique_ptr<Expression> expr);
    std::unique_ptr<Statement> optimizeStatement(std::unique_ptr<Statement> stmt);
    
    // Constant folding pass
    void constantFoldingPass(std::vector<std::unique_ptr<Statement>>& statements);
    
    // Dead code elimination pass
    void deadCodeEliminationPass(std::vector<std::unique_ptr<Statement>>& statements);
};
