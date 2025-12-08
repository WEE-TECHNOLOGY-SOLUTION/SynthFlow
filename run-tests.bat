@echo off
echo ========================================
echo   SynthFlow Test Runner
echo ========================================
echo.

REM Compile all tests
echo Compiling tests...
g++ -std=c++17 -Icompiler/include tests/test_lexer.cpp compiler/src/lexer/lexer.cpp -o test_lexer.exe
g++ -std=c++17 -Icompiler/include tests/test_parser.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/codegen/code_generator.cpp -o test_parser.exe
g++ -std=c++17 -Icompiler/include tests/test_semantic.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp -o test_semantic.exe
g++ -std=c++17 -Icompiler/include tests/test_codegen.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/codegen/code_generator.cpp -o test_codegen.exe
g++ -std=c++17 -Icompiler/include tests/test_while_loop.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/codegen/code_generator.cpp -o test_while_loop.exe
g++ -std=c++17 -Icompiler/include tests/test_break_continue.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp compiler/src/codegen/code_generator.cpp -o test_break_continue.exe
g++ -std=c++17 -Icompiler/include tests/test_for_loop.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/codegen/code_generator.cpp -o test_for_loop.exe
g++ -std=c++17 -Icompiler/include tests/test_arrays.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp compiler/src/codegen/code_generator.cpp -o test_arrays.exe

echo.
echo Running tests...
echo.

REM Run lexer test
if exist test_lexer.exe (
    echo Testing lexer...
    .\test_lexer.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Lexer test passed!
    ) else (
        echo [FAIL] Lexer test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [ERROR] Lexer test executable not found!
    exit /b 1
)

echo.

REM Run parser test
if exist test_parser.exe (
    echo Testing parser...
    .\test_parser.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Parser test passed!
    ) else (
        echo [FAIL] Parser test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] Parser test executable not found!
)

echo.

REM Run semantic analyzer test
if exist test_semantic.exe (
    echo Testing semantic analyzer...
    .\test_semantic.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Semantic analyzer test passed!
    ) else (
        echo [FAIL] Semantic analyzer test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] Semantic analyzer test executable not found!
)

echo.

REM Run code generator test
if exist test_codegen.exe (
    echo Testing code generator...
    .\test_codegen.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Code generator test passed!
    ) else (
        echo [FAIL] Code generator test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] Code generator test executable not found!
)

echo.

REM Run while loop test
if exist test_while_loop.exe (
    echo Testing while loop...
    .\test_while_loop.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] While loop test passed!
    ) else (
        echo [FAIL] While loop test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] While loop test executable not found!
)

echo.

REM Run break/continue test
if exist test_break_continue.exe (
    echo Testing break/continue...
    .\test_break_continue.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Break/continue test passed!
    ) else (
        echo [FAIL] Break/continue test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] Break/continue test executable not found!
)

echo.

REM Run for loop test
if exist test_for_loop.exe (
    echo Testing for loop...
    .\test_for_loop.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] For loop test passed!
    ) else (
        echo [FAIL] For loop test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] For loop test executable not found!
)

echo.

REM Run arrays test
if exist test_arrays.exe (
    echo Testing arrays...
    .\test_arrays.exe
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Arrays test passed!
    ) else (
        echo [FAIL] Arrays test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo [WARN] Arrays test executable not found!
)

echo.
echo ========================================
echo   All tests completed!
echo ========================================
echo.
echo Press any key to exit...
pause >nul