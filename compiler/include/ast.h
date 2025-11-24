#pragma once
#include <memory>
#include <vector>
#include <string>

// Forward declarations
class ASTVisitor;

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

// Expressions
class Expression : public ASTNode {};

class IntegerLiteral : public Expression {
public:
    int64_t value;
    explicit IntegerLiteral(int64_t val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

class FloatLiteral : public Expression {
public:
    double value;
    explicit FloatLiteral(double val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

class StringLiteral : public Expression {
public:
    std::string value;
    explicit StringLiteral(std::string val) : value(std::move(val)) {}
    void accept(ASTVisitor* visitor) override;
};

class BooleanLiteral : public Expression {
public:
    bool value;
    explicit BooleanLiteral(bool val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

class Identifier : public Expression {
public:
    std::string name;
    explicit Identifier(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor* visitor) override;
};

// Binary Operations
class BinaryOp : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    enum OpType { ADD, SUB, MUL, DIV, MOD, EQ, NE, LT, GT, LE, GE, AND, OR };
    OpType op;
    
    BinaryOp(std::unique_ptr<Expression> l, OpType o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    void accept(ASTVisitor* visitor) override;
};

// Statements
class Statement : public ASTNode {};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    explicit ExpressionStatement(std::unique_ptr<Expression> expr) 
        : expression(std::move(expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> initializer;
    
    VariableDeclaration(std::string n, std::unique_ptr<Expression> init)
        : name(std::move(n)), initializer(std::move(init)) {}
    void accept(ASTVisitor* visitor) override;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<Statement> body;
    
    FunctionDeclaration(std::string n, std::vector<std::string> params, 
                       std::unique_ptr<Statement> b)
        : name(std::move(n)), parameters(std::move(params)), body(std::move(b)) {}
    void accept(ASTVisitor* visitor) override;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    void addStatement(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }
    void accept(ASTVisitor* visitor) override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch; // Optional
    
    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> thenB,
               std::unique_ptr<Statement> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    void accept(ASTVisitor* visitor) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> returnValue;
    
    explicit ReturnStatement(std::unique_ptr<Expression> retVal = nullptr)
        : returnValue(std::move(retVal)) {}
    void accept(ASTVisitor* visitor) override;
};

// Visitor Pattern
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BooleanLiteral* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryOp* node) = 0;
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(VariableDeclaration* node) = 0;
    virtual void visit(FunctionDeclaration* node) = 0;
    virtual void visit(BlockStatement* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
};