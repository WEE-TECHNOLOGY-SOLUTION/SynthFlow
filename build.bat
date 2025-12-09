@echo off
echo Building SynthFlow with maximum optimizations...

g++ -std=c++17 -Wall -Wextra -O3 -march=native -flto -DNDEBUG -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    compiler/src/codegen/code_generator.cpp ^
    compiler/src/codegen/js_transpiler.cpp ^
    compiler/src/interpreter/interpreter.cpp ^
    compiler/src/optimizer/optimizer.cpp ^
    compiler/src/bytecode/bytecode_compiler.cpp ^
    compiler/src/bytecode/vm.cpp ^
    compiler/src/http/http_client.cpp ^
    compiler/src/main.cpp ^
    -lwinhttp ^
    -o synthflow.exe

echo Building SynthFlow LSP...
g++ -std=c++17 -Wall -Wextra -O3 -march=native -flto -DNDEBUG -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    lsp-server/src/main.cpp ^
    -o synthflow-lsp.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run ./synthflow.exe to use the compiler.
) else (
    echo Build failed.
    exit /b %ERRORLEVEL%
)
