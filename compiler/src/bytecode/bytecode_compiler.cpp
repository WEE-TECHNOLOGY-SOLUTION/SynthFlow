#include "../../include/bytecode_compiler.h"
#include <stdexcept>

uint32_t BytecodeCompiler::resolveVariable(const std::string& name) {
    if (inFunction) {
        auto it = localVariables.find(name);
        if (it != localVariables.end()) {
            return it->second;
        }
    }
    auto it = globalVariables.find(name);
    if (it != globalVariables.end()) {
        return it->second;
    }
    throw std::runtime_error("Undefined variable: " + name);
}

uint32_t BytecodeCompiler::declareVariable(const std::string& name) {
    if (inFunction) {
        uint32_t index = nextLocalIndex++;
        localVariables[name] = index;
        return index;
    } else {
        uint32_t index = static_cast<uint32_t>(globalVariables.size());
        globalVariables[name] = index;
        return index;
    }
}

BytecodeChunk BytecodeCompiler::compile(const std::vector<std::unique_ptr<Statement>>& statements) {
    chunk = BytecodeChunk();
    
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    
    chunk.emit(OpCode::HALT);
    return chunk;
}

void BytecodeCompiler::visit(IntegerLiteral* node) {
    uint32_t index = chunk.addConstant(node->value);
    chunk.emit(OpCode::PUSH_INT, index);
}

void BytecodeCompiler::visit(FloatLiteral* node) {
    uint32_t index = chunk.addConstant(node->value);
    chunk.emit(OpCode::PUSH_FLOAT, index);
}

void BytecodeCompiler::visit(StringLiteral* node) {
    uint32_t index = chunk.addConstant(node->value);
    chunk.emit(OpCode::PUSH_STRING, index);
}

void BytecodeCompiler::visit(BooleanLiteral* node) {
    uint32_t index = chunk.addConstant(node->value);
    chunk.emit(OpCode::PUSH_BOOL, index);
}

void BytecodeCompiler::visit(NullLiteral* node) {
    (void)node;
    chunk.emit(OpCode::PUSH_NULL);
}

void BytecodeCompiler::visit(Identifier* node) {
    uint32_t index = resolveVariable(node->name);
    if (inFunction) {
        chunk.emit(OpCode::LOAD_VAR, index);
    } else {
        chunk.emit(OpCode::LOAD_GLOBAL, index);
    }
}

void BytecodeCompiler::visit(BinaryExpression* node) {
    node->left->accept(*this);
    node->right->accept(*this);
    
    if (node->op == "+") chunk.emit(OpCode::ADD);
    else if (node->op == "-") chunk.emit(OpCode::SUB);
    else if (node->op == "*") chunk.emit(OpCode::MUL);
    else if (node->op == "/") chunk.emit(OpCode::DIV);
    else if (node->op == "%") chunk.emit(OpCode::MOD);
    else if (node->op == "==") chunk.emit(OpCode::EQ);
    else if (node->op == "!=") chunk.emit(OpCode::NE);
    else if (node->op == "<") chunk.emit(OpCode::LT);
    else if (node->op == ">") chunk.emit(OpCode::GT);
    else if (node->op == "<=") chunk.emit(OpCode::LE);
    else if (node->op == ">=") chunk.emit(OpCode::GE);
    else if (node->op == "&&" || node->op == "and") chunk.emit(OpCode::AND);
    else if (node->op == "||" || node->op == "or") chunk.emit(OpCode::OR);
}

void BytecodeCompiler::visit(UnaryExpression* node) {
    node->operand->accept(*this);
    
    if (node->op == "-") chunk.emit(OpCode::NEG);
    else if (node->op == "!" || node->op == "not") chunk.emit(OpCode::NOT);
}

void BytecodeCompiler::visit(AssignmentExpression* node) {
    node->right->accept(*this);
    
    if (auto* id = dynamic_cast<Identifier*>(node->left.get())) {
        uint32_t index = resolveVariable(id->name);
        if (inFunction) {
            chunk.emit(OpCode::STORE_VAR, index);
        } else {
            chunk.emit(OpCode::STORE_GLOBAL, index);
        }
    }
}

void BytecodeCompiler::visit(CallExpression* node) {
    // Push arguments
    for (auto& arg : node->arguments) {
        arg->accept(*this);
    }
    
    // Emit call with argument count
    uint32_t argCount = static_cast<uint32_t>(node->arguments.size());
    chunk.emit(OpCode::CALL, argCount);
}

void BytecodeCompiler::visit(ArrayLiteral* node) {
    for (auto& elem : node->elements) {
        elem->accept(*this);
    }
    chunk.emit(OpCode::MAKE_ARRAY, static_cast<uint32_t>(node->elements.size()));
}

void BytecodeCompiler::visit(ArrayIndexExpression* node) {
    node->array->accept(*this);
    node->index->accept(*this);
    chunk.emit(OpCode::INDEX);
}

void BytecodeCompiler::visit(ArrayAssignmentExpression* node) {
    node->array->accept(*this);
    node->index->accept(*this);
    node->value->accept(*this);
    chunk.emit(OpCode::INDEX_SET);
}

void BytecodeCompiler::visit(VariableDeclaration* node) {
    if (node->initializer) {
        node->initializer->accept(*this);
    } else {
        chunk.emit(OpCode::PUSH_NULL);
    }
    
    uint32_t index = declareVariable(node->name);
    if (inFunction) {
        chunk.emit(OpCode::STORE_VAR, index);
    } else {
        chunk.emit(OpCode::STORE_GLOBAL, index);
    }
}

void BytecodeCompiler::visit(ExpressionStatement* node) {
    node->expression->accept(*this);
    chunk.emit(OpCode::POP);
}

void BytecodeCompiler::visit(BlockStatement* node) {
    for (auto& stmt : node->statements) {
        stmt->accept(*this);
    }
}

void BytecodeCompiler::visit(IfStatement* node) {
    node->condition->accept(*this);
    
    size_t jumpIfFalse = chunk.emit(OpCode::JUMP_IF_FALSE, 0);
    
    node->thenBranch->accept(*this);
    
    if (node->elseBranch) {
        size_t jumpOver = chunk.emit(OpCode::JUMP, 0);
        chunk.patchJump(jumpIfFalse, static_cast<uint32_t>(chunk.code.size()));
        node->elseBranch->accept(*this);
        chunk.patchJump(jumpOver, static_cast<uint32_t>(chunk.code.size()));
    } else {
        chunk.patchJump(jumpIfFalse, static_cast<uint32_t>(chunk.code.size()));
    }
}

void BytecodeCompiler::visit(WhileStatement* node) {
    size_t loopStart = chunk.code.size();
    
    node->condition->accept(*this);
    size_t exitJump = chunk.emit(OpCode::JUMP_IF_FALSE, 0);
    
    node->body->accept(*this);
    chunk.emit(OpCode::JUMP, static_cast<uint32_t>(loopStart));
    
    chunk.patchJump(exitJump, static_cast<uint32_t>(chunk.code.size()));
}

void BytecodeCompiler::visit(ForStatement* node) {
    if (node->initializer) {
        node->initializer->accept(*this);
    }
    
    size_t loopStart = chunk.code.size();
    
    if (node->condition) {
        node->condition->accept(*this);
    } else {
        chunk.emit(OpCode::PUSH_BOOL, chunk.addConstant(true));
    }
    
    size_t exitJump = chunk.emit(OpCode::JUMP_IF_FALSE, 0);
    
    node->body->accept(*this);
    
    if (node->increment) {
        node->increment->accept(*this);
        chunk.emit(OpCode::POP);
    }
    
    chunk.emit(OpCode::JUMP, static_cast<uint32_t>(loopStart));
    chunk.patchJump(exitJump, static_cast<uint32_t>(chunk.code.size()));
}

void BytecodeCompiler::visit(BreakStatement* node) {
    (void)node;
    // TODO: Implement break with jump stack
}

void BytecodeCompiler::visit(ContinueStatement* node) {
    (void)node;
    // TODO: Implement continue with jump stack
}

void BytecodeCompiler::visit(FunctionDeclaration* node) {
    // TODO: Compile function to separate chunk
    (void)node;
}

void BytecodeCompiler::visit(ReturnStatement* node) {
    if (node->value) {
        node->value->accept(*this);
    } else {
        chunk.emit(OpCode::PUSH_NULL);
    }
    chunk.emit(OpCode::RETURN);
}

void BytecodeCompiler::visit(TryStatement* node) {
    // Simplified try/catch - just execute try block for now
    if (node->tryBlock) {
        node->tryBlock->accept(*this);
    }
}

void BytecodeCompiler::visit(LambdaExpression* node) {
    (void)node;
    // TODO: Implement lambda bytecode
}

void BytecodeCompiler::visit(MatchExpression* node) {
    (void)node;
    // TODO: Implement match bytecode
}

void BytecodeCompiler::visit(CompoundAssignment* node) {
    (void)node;
    // TODO: Implement compound assignment bytecode
}

void BytecodeCompiler::visit(UpdateExpression* node) {
    (void)node;
    // TODO: Implement update expression bytecode
}

void BytecodeCompiler::visit(InterpolatedString* node) {
    (void)node;
    // TODO: Implement interpolated string bytecode
}
