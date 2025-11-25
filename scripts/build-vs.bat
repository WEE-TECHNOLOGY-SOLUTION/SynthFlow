@echo off
echo Building SynthFlow compiler with Visual Studio...
cd /d D:\SynthFlow\synthflow

REM Initialize Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Compile all source files directly into an executable
echo Compiling and linking...
cl /std:c++17 /Icompiler/include /EHsc compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp compiler/src/codegen/code_generator.cpp compiler/src/main.cpp /link /out:synthflow.exe

REM Check if build was successful
if exist synthflow.exe (
    echo.
    echo Build successful!
    echo.
    echo To run an example:
    echo synthflow.exe examples/hello.synth
    echo.
) else (
    echo.
    echo Build failed!
    echo.
    exit /b 1
)