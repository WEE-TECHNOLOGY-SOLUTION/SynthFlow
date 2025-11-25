@echo off
echo Building SynthFlow compiler directly...
cd /d D:\SynthFlow\synthflow

REM Initialize Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Compile all source files
echo Compiling source files...
set compileFlags=/std:c++17 /Icompiler/include

REM Compile individual source files to object files
cl %compileFlags% /c compiler/src/lexer/lexer.cpp /Folexer.obj
cl %compileFlags% /c compiler/src/parser/parser.cpp /Foparser.obj
cl %compileFlags% /c compiler/src/ast/ast_visitor.cpp /Foast_visitor.obj
cl %compileFlags% /c compiler/src/semantic/semantic_analyzer.cpp /Fosemantic_analyzer.obj
cl %compileFlags% /c compiler/src/codegen/code_generator.cpp /Focode_generator.obj
cl %compileFlags% /c compiler/src/main.cpp /Fomain.obj

REM Link all object files into the final executable
echo Linking object files...
link lexer.obj parser.obj ast_visitor.obj semantic_analyzer.obj code_generator.obj main.obj /OUT:synthflow.exe

REM Check if build was successful
if exist synthflow.exe (
    echo Build successful!
    echo Run "synthflow.exe examples/hello.synth" to test
) else (
    echo Build failed!
    exit /b 1
)