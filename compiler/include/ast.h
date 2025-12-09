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
class NullLiteral;
class TryStatement;
class LambdaExpression;
class MatchExpression;
class CompoundAssignment;
class UpdateExpression;
class InterpolatedString;

// Base visitor class
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expression visitors
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BooleanLiteral* node) = 0;
    virtual void visit(NullLiteral* node) = 0;  // Null safety
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(AssignmentExpression* node) = 0;
    virtual void visit(CallExpression* node) = 0;
    virtual void visit(ArrayLiteral* node) = 0;
    virtual void visit(ArrayIndexExpression* node) = 0;
    virtual void visit(ArrayAssignmentExpression* node) = 0;
    virtual void visit(LambdaExpression* node) = 0;  // Lambda functions
    virtual void visit(MatchExpression* node) = 0;   // Match expression
    virtual void visit(CompoundAssignment* node) = 0; // +=, -=, etc.
    virtual void visit(UpdateExpression* node) = 0;  // ++, --
    virtual void visit(InterpolatedString* node) = 0; // String interpolation
    
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
    virtual void visit(TryStatement* node) = 0;  // Error handling
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

// Null literal node (for null safety)
class NullLiteral : public Expression {
public:
    NullLiteral() = default;
    
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

// Variable declaration statement (with safety features)
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> initializer;
    bool isConst = false;           // const keyword (immutable)
    std::string typeName = "";      // Optional type annotation (int, float, string, bool, array)
    bool isNullable = false;        // Whether type is nullable (?)
    
    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init)
        : name(n), initializer(std::move(init)) {}
    
    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init, 
                        bool constant, const std::string& type = "", bool nullable = false)
        : name(n), initializer(std::move(init)), isConst(constant), 
          typeName(type), isNullable(nullable) {}
    
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

// Try/Catch statement for error handling
class TryStatement : public Statement {
public:
    std::unique_ptr<BlockStatement> tryBlock;
    std::string errorVariable;  // Variable name in catch (e.g., "error" in "catch (error)")
    std::unique_ptr<BlockStatement> catchBlock;
    
    TryStatement(std::unique_ptr<BlockStatement> tryB,
                 const std::string& errVar,
                 std::unique_ptr<BlockStatement> catchB)
        : tryBlock(std::move(tryB)), errorVariable(errVar), catchBlock(std::move(catchB)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Lambda expression: (x, y) => x + y
class LambdaExpression : public Expression {
public:
    std::vector<std::string> parameters;
    std::unique_ptr<Expression> body;  // Single expression body
    std::unique_ptr<BlockStatement> blockBody;  // Optional block body
    
    LambdaExpression(std::vector<std::string> params, std::unique_ptr<Expression> expr)
        : parameters(std::move(params)), body(std::move(expr)), blockBody(nullptr) {}
    
    LambdaExpression(std::vector<std::string> params, std::unique_ptr<BlockStatement> block)
        : parameters(std::move(params)), body(nullptr), blockBody(std::move(block)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Match case for pattern matching
struct MatchCase {
    std::unique_ptr<Expression> pattern;  // nullptr means default case (_)
    std::unique_ptr<Expression> result;
};

// Match expression: match x { 1 => "one", 2 => "two", _ => "other" }
class MatchExpression : public Expression {
public:
    std::unique_ptr<Expression> subject;
    std::vector<MatchCase> cases;
    
    MatchExpression(std::unique_ptr<Expression> subj, std::vector<MatchCase> c)
        : subject(std::move(subj)), cases(std::move(c)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Compound assignment: x += 5, x -= 3, x *= 2, x /= 4
class CompoundAssignment : public Expression {
public:
    std::unique_ptr<Expression> target;
    std::string op;  // "+=", "-=", "*=", "/="
    std::unique_ptr<Expression> value;
    
    CompoundAssignment(std::unique_ptr<Expression> tgt, const std::string& o, std::unique_ptr<Expression> val)
        : target(std::move(tgt)), op(o), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Update expression: x++ or x--
class UpdateExpression : public Expression {
public:
    std::unique_ptr<Expression> operand;
    std::string op;  // "++" or "--"
    bool prefix;     // true for ++x, false for x++
    
    UpdateExpression(std::unique_ptr<Expression> expr, const std::string& o, bool pre)
        : operand(std::move(expr)), op(o), prefix(pre) {}
    
    void accept(ASTVisitor& visitor) override;
};

// String part for interpolated strings
struct StringPart {
    bool isExpression;
    std::string text;  // For literal parts
    std::unique_ptr<Expression> expr;  // For ${expr} parts
    
    StringPart(const std::string& t) : isExpression(false), text(t), expr(nullptr) {}
    StringPart(std::unique_ptr<Expression> e) : isExpression(true), text(""), expr(std::move(e)) {}
};

// Interpolated string: "Hello, ${name}!"
class InterpolatedString : public Expression {
public:
    std::vector<StringPart> parts;
    
    explicit InterpolatedString(std::vector<StringPart> p) : parts(std::move(p)) {}
    
    void accept(ASTVisitor& visitor) override;
};

#endif // AST_H