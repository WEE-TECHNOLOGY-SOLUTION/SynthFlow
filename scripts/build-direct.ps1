# PowerShell script to build SynthFlow compiler directly without make
Write-Host "Building SynthFlow compiler directly..."
Set-Location "D:\SynthFlow\synthflow"

# Initialize Visual Studio environment
$vsPath = "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if (Test-Path $vsPath) {
    Write-Host "Initializing Visual Studio environment..."
    & $vsPath
}

# Compile all source files
Write-Host "Compiling source files..."
$compileFlags = "/std:c++17 /Icompiler/include"

# Compile individual source files to object files
cl $compileFlags /c compiler/src/lexer/lexer.cpp /Folexer.obj
cl $compileFlags /c compiler/src/parser/parser.cpp /Foparser.obj
cl $compileFlags /c compiler/src/ast/ast_visitor.cpp /Foast_visitor.obj
cl $compileFlags /c compiler/src/semantic/semantic_analyzer.cpp /Fosemantic_analyzer.obj
cl $compileFlags /c compiler/src/codegen/code_generator.cpp /Focode_generator.obj
cl $compileFlags /c compiler/src/main.cpp /Fomain.obj

# Link all object files into the final executable
Write-Host "Linking object files..."
link lexer.obj parser.obj ast_visitor.obj semantic_analyzer.obj code_generator.obj main.obj /OUT:synthflow.exe

# Check if build was successful
if (Test-Path "synthflow.exe") {
    Write-Host "Build successful!" -ForegroundColor Green
    Write-Host "Run .\synthflow.exe examples/hello.synth to test" -ForegroundColor Yellow
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}