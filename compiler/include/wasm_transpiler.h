#ifndef WASM_TRANSPILER_H
#define WASM_TRANSPILER_H

#include "ast.h"
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <map>

/**
 * WasmTranspiler - Generates WebAssembly Text (WAT) format from SynthFlow AST.
 * 
 * The generated WAT can be compiled to binary .wasm using tools like wat2wasm.
 * This initial implementation uses f64 for all numeric types for simplicity.
 */
class WasmTranspiler : public ASTVisitor {
private:
    std::ostringstream output;
    int indentLevel = 0;
    
    // Variable management - maps variable names to local indices
    std::map<std::string, int> localVariables;
    int nextLocalIndex = 0;
    
    // Function management
    std::vector<std::string> functionNames;
    bool inFunction = false;
    
    // Helper methods
    void indent();
    void emit(const std::string& code);
    void emitLine(const std::string& code);
    
    // Get or create a local variable index
    int getLocalIndex(const std::string& name);
    
public:
    WasmTranspiler() = default;
    
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
    
    // SADK Expression visitors
    void visit(MapLiteral* node) override;
    void visit(MemberExpression* node) override;
    void visit(SelfExpression* node) override;
    
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
    
    // SADK Statement visitors
    void visit(ImportStatement* node) override;
    void visit(StructDeclaration* node) override;
};

#endif // WASM_TRANSPILER_H
