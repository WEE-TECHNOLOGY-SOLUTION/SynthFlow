@echo off
echo Running SynthFlow tests...
cd /d D:\SynthFlow\synthflow

REM Run lexer test
if exist test_lexer.exe (
    echo Testing lexer...
    test_lexer.exe
    if %ERRORLEVEL% EQU 0 (
        echo Lexer test passed!
    ) else (
        echo Lexer test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo Error: test_lexer.exe not found
    exit /b 1
)

REM Run parser test
if exist test_parser.exe (
    echo Testing parser...
    test_parser.exe
    if %ERRORLEVEL% EQU 0 (
        echo Parser test passed!
    ) else (
        echo Parser test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo Warning: test_parser.exe not found
)

REM Run semantic analyzer test
if exist test_semantic.exe (
    echo Testing semantic analyzer...
    test_semantic.exe
    if %ERRORLEVEL% EQU 0 (
        echo Semantic analyzer test passed!
    ) else (
        echo Semantic analyzer test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo Warning: test_semantic.exe not found
)

REM Run code generator test
if exist test_codegen.exe (
    echo Testing code generator...
    test_codegen.exe
    if %ERRORLEVEL% EQU 0 (
        echo Code generator test passed!
    ) else (
        echo Code generator test failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo Warning: test_codegen.exe not found
)

echo All tests completed!