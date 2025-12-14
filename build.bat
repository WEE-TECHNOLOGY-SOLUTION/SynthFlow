@echo off
echo Building SynthFlow v0.0.30 with Web Framework...

g++ -std=c++17 -Wall -Wextra -Wno-unused-parameter -O3 -march=native -flto -DNDEBUG -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    compiler/src/codegen/code_generator.cpp ^
    compiler/src/codegen/js_transpiler.cpp ^
    compiler/src/interpreter/interpreter.cpp ^
    compiler/src/http/http_client.cpp ^
    compiler/src/http/http_server.cpp ^
    compiler/src/main.cpp ^
    -lwinhttp -lws2_32 ^
    -o synthflow-0.0.30.exe

if %ERRORLEVEL% NEQ 0 (
    echo Main build failed.
    exit /b %ERRORLEVEL%
)

echo Building SynthFlow LSP...
g++ -std=c++17 -Wall -Wextra -Wno-unused-parameter -O3 -march=native -flto -DNDEBUG -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    lsp-server/src/main.cpp ^
    -o synthflow-lsp.exe

echo Building SynthFlow MCP...
g++ -std=c++17 -Wall -Wextra -Wno-unused-parameter -O3 -march=native -flto -DNDEBUG -Icompiler/include ^
    compiler/src/lexer/lexer.cpp ^
    compiler/src/parser/parser.cpp ^
    compiler/src/ast/ast_visitor.cpp ^
    compiler/src/semantic/semantic_analyzer.cpp ^
    mcp-server/src/main.cpp ^
    -o synthflow-mcp.exe

echo.
echo Build successful! Run ./synthflow-0.0.30.exe to use the compiler.
echo.
echo New CLI Features (Python Parity):
echo   synthflow --help          Show all commands
echo   synthflow --version       Show version
echo   synthflow -c "code"       Execute inline code
echo   synthflow -m module       Run module as script  
echo   synthflow -v run file.sf  Verbose mode
echo   synthflow -q run file.sf  Quiet mode
echo   synthflow -O run file.sf  Optimization mode
echo   synthflow repl            Start interactive REPL
echo   synthflow run file.sf     Execute program
echo   synthflow check file.sf   Type-check only
echo   synthflow compile file.sf Show tokens and analysis
echo   synthflow transpile file.sf Convert to JavaScript
