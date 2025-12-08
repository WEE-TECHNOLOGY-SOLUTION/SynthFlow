#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <utility>  // For std::move

// Forward declarations for all AST node classes
class ASTVisitor;
class ASTNode;
class Expression;
class Statement;
class IntegerLiteral;
class FloatLiteral;
class StringLiteral;
class BooleanLiteral;
class Identifier;
class BinaryExpression;
class UnaryExpression;
class VariableDeclaration;
class AssignmentExpression;
class ExpressionStatement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class ForStatement;
class BreakStatement;
class ContinueStatement;
class FunctionDeclaration;
class ReturnStatement;
class CallExpression;
class ArrayLiteral;
class ArrayIndexExpression;
class ArrayAssignmentExpression;

// Base visitor class
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expression visitors
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BooleanLiteral* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(AssignmentExpression* node) = 0;
    virtual void visit(CallExpression* node) = 0;
    virtual void visit(ArrayLiteral* node) = 0;
    virtual void visit(ArrayIndexExpression* node) = 0;
    virtual void visit(ArrayAssignmentExpression* node) = 0;
    
    // Statement visitors
    virtual void visit(VariableDeclaration* node) = 0;
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(BlockStatement* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(WhileStatement* node) = 0;
    virtual void visit(ForStatement* node) = 0;
    virtual void visit(BreakStatement* node) = 0;
    virtual void visit(ContinueStatement* node) = 0;
    virtual void visit(FunctionDeclaration* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
};

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Base class for expressions
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// Base class for statements
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// Integer literal node
class IntegerLiteral : public Expression {
public:
    int64_t value;
    
    IntegerLiteral() : value(0) {}
    explicit IntegerLiteral(int64_t val) : value(val) {}
    
    void accept(ASTVisitor& visitor) override;
};

// String literal node
class StringLiteral : public Expression {
public:
    std::string value;
    
    StringLiteral() : value("") {}
    explicit StringLiteral(const std::string& val) : value(val) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Boolean literal node
class BooleanLiteral : public Expression {
public:
    bool value;
    
    BooleanLiteral() : value(false) {}
    explicit BooleanLiteral(bool val) : value(val) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Float literal node
class FloatLiteral : public Expression {
public:
    double value;
    
    FloatLiteral() : value(0.0) {}
    explicit FloatLiteral(double val) : value(val) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Identifier node
class Identifier : public Expression {
public:
    std::string name;
    
    Identifier() : name("") {}
    explicit Identifier(const std::string& n) : name(n) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Binary expression node
class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(std::unique_ptr<Expression> l, const std::string& o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Unary expression node
class UnaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const std::string& o, std::unique_ptr<Expression> expr)
        : op(o), operand(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Variable declaration statement
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> initializer;
    
    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init)
        : name(n), initializer(std::move(init)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Assignment expression
class AssignmentExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
    AssignmentExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Expression statement
class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    explicit ExpressionStatement(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Block statement
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement() = default;
    
    void accept(ASTVisitor& visitor) override;
};

// If statement
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> thenBranch;
    std::unique_ptr<BlockStatement> elseBranch;
    
    IfStatement(std::unique_ptr<Expression> cond,
                std::unique_ptr<BlockStatement> thenB,
                std::unique_ptr<BlockStatement> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// While statement
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;
    
    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStatement> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// For statement
class ForStatement : public Statement {
public:
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> increment;
    std::unique_ptr<BlockStatement> body;
    
    ForStatement(std::unique_ptr<Statement> init,
                 std::unique_ptr<Expression> cond,
                 std::unique_ptr<Expression> incr,
                 std::unique_ptr<BlockStatement> b)
        : initializer(std::move(init)), condition(std::move(cond)),
          increment(std::move(incr)), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Break statement
class BreakStatement : public Statement {
public:
    BreakStatement() = default;
    
    void accept(ASTVisitor& visitor) override;
};

// Continue statement
class ContinueStatement : public Statement {
public:
    ContinueStatement() = default;
    
    void accept(ASTVisitor& visitor) override;
};

// Function declaration
class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStatement> body;
    
    FunctionDeclaration(const std::string& n,
                        const std::vector<std::string>& params,
                        std::unique_ptr<BlockStatement> b)
        : name(n), parameters(params), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Return statement
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    explicit ReturnStatement(std::unique_ptr<Expression> val = nullptr)
        : value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Function call expression
class CallExpression : public Expression {
public:
    std::string callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(const std::string& c,
                   std::vector<std::unique_ptr<Expression>> args)
        : callee(c), arguments(std::move(args)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Array literal expression
class ArrayLiteral : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> elements;
    
    ArrayLiteral() = default;
    
    void accept(ASTVisitor& visitor) override;
};

// Array indexing expression
class ArrayIndexExpression : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;
    
    ArrayIndexExpression(std::unique_ptr<Expression> arr,
                         std::unique_ptr<Expression> idx)
        : array(std::move(arr)), index(std::move(idx)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Array assignment expression
class ArrayAssignmentExpression : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;
    std::unique_ptr<Expression> value;
    
    ArrayAssignmentExpression(std::unique_ptr<Expression> arr,
                              std::unique_ptr<Expression> idx,
                              std::unique_ptr<Expression> val)
        : array(std::move(arr)), index(std::move(idx)), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override;
};

#endif // AST_H