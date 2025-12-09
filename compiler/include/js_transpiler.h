#ifndef JS_TRANSPILER_H
#define JS_TRANSPILER_H

#include "ast.h"
#include <string>
#include <sstream>
#include <memory>
#include <vector>

class JSTranspiler : public ASTVisitor {
private:
    std::ostringstream output;
    int indentLevel = 0;
    
    void indent();
    void emit(const std::string& code);
    void emitLine(const std::string& code);
    
public:
    JSTranspiler() = default;
    
    // Main transpile function
    std::string transpile(const std::vector<std::unique_ptr<Statement>>& statements);
    
    // Expression visitors
    void visit(IntegerLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(NullLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(AssignmentExpression* node) override;
    void visit(CallExpression* node) override;
    void visit(ArrayLiteral* node) override;
    void visit(ArrayIndexExpression* node) override;
    void visit(ArrayAssignmentExpression* node) override;
    void visit(LambdaExpression* node) override;
    void visit(MatchExpression* node) override;
    void visit(CompoundAssignment* node) override;
    void visit(UpdateExpression* node) override;
    void visit(InterpolatedString* node) override;
    
    // Statement visitors
    void visit(VariableDeclaration* node) override;
    void visit(ExpressionStatement* node) override;
    void visit(BlockStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ForStatement* node) override;
    void visit(BreakStatement* node) override;
    void visit(ContinueStatement* node) override;
    void visit(FunctionDeclaration* node) override;
    void visit(ReturnStatement* node) override;
    void visit(TryStatement* node) override;
};

#endif // JS_TRANSPILER_H
