@echo off
echo Building SynthFlow...

g++ -std=c++17 -Wall -Wextra -O2 -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    compiler/src/codegen/code_generator.cpp ^
    compiler/src/codegen/js_transpiler.cpp ^
    compiler/src/interpreter/interpreter.cpp ^
    compiler/src/main.cpp ^
    -o synthflow.exe

echo Building SynthFlow LSP...
g++ -std=c++17 -Wall -Wextra -O2 -Icompiler/include ^
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
