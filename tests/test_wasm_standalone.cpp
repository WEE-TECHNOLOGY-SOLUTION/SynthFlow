// Standalone test for WasmTranspiler
// Compile with: g++ -std=c++17 -I compiler/include tests/test_wasm_standalone.cpp compiler/src/lexer/lexer.cpp compiler/src/parser/parser.cpp compiler/src/ast/ast_visitor.cpp compiler/src/semantic/semantic_analyzer.cpp compiler/src/codegen/wasm_transpiler.cpp -o test_wasm.exe

#include "../compiler/include/lexer.h"
#include "../compiler/include/parser.h"
#include "../compiler/include/semantic_analyzer.h"
#include "../compiler/include/wasm_transpiler.h"
#include <iostream>
#include <string>

int main() {
    std::string source = R"(
        fn add(a, b) {
            return a + b
        }
        
        let x = 10
        let y = 20
        let result = add(x, y)
        print(result)
    )";
    
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        WasmTranspiler transpiler;
        std::string watCode = transpiler.transpile(statements);
        
        std::cout << watCode << std::endl;
        std::cout << "\n=== WasmTranspiler Test PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
