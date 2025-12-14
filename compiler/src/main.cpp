#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/semantic_analyzer.h"
#include "../include/code_generator.h"
#include "../include/interpreter.h"
#include "../include/js_transpiler.h"
#include "../include/modules.h"
#include "../include/CLI11.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#define SYNTHFLOW_VERSION "0.0.27"

// =============================================================================
// Global Configuration
// =============================================================================

struct Config {
    bool verbose = false;
    bool quiet = false;
    int optimizeLevel = 0;
    bool interactive = false;
};

static Config g_config;

// =============================================================================
// Logging Utilities
// =============================================================================

void logInfo(const std::string& msg) {
    if (g_config.verbose && !g_config.quiet) {
        std::cerr << "\033[36m[INFO]\033[0m " << msg << std::endl;
    }
}

void logDebug(const std::string& msg) {
    if (g_config.verbose) {
        std::cerr << "\033[35m[DEBUG]\033[0m " << msg << std::endl;
    }
}

void logError(const std::string& msg) {
    if (!g_config.quiet) {
        std::cerr << "\033[31mError:\033[0m " << msg << std::endl;
    }
}

void logSuccess(const std::string& msg) {
    if (!g_config.quiet) {
        std::cerr << "\033[32m✓\033[0m " << msg << std::endl;
    }
}

// =============================================================================
// File Utilities
// =============================================================================

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// =============================================================================
// Core Execution Functions
// =============================================================================

int runProgram(const std::string& source) {
    try {
        logDebug("Starting lexer...");
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        logInfo("Tokenized " + std::to_string(tokens.size()) + " tokens");
        
        logDebug("Starting parser...");
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        logInfo("Parsed " + std::to_string(statements.size()) + " statements");
        
        // Skip semantic analysis in -O mode for speed
        if (g_config.optimizeLevel < 1) {
            logDebug("Running semantic analysis...");
            SemanticAnalyzer analyzer;
            analyzer.analyze(statements);
            logInfo("Semantic analysis passed");
        } else {
            logInfo("Skipping semantic analysis (optimization mode)");
        }
        
        logDebug("Starting interpreter...");
        Interpreter interpreter;
        interpreter.execute(statements);
        
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}

int compileProgram(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (!g_config.quiet) {
            std::cout << "=== Tokens ===" << std::endl;
            for (const auto& token : tokens) {
                std::cout << "Line " << token.line << ", Col " << token.column 
                          << ": " << token.lexeme << " (" << static_cast<int>(token.type) << ")" << std::endl;
            }
        }
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        if (!g_config.quiet) {
            std::cout << "\n=== Parse Successful ===" << std::endl;
            std::cout << "Parsed " << statements.size() << " statements" << std::endl;
        }
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        if (!g_config.quiet) {
            std::cout << "\n=== Semantic Analysis Successful ===" << std::endl;
        }
        
        CodeGenerator generator;
        std::string generatedCode = generator.generate(statements);
        
        if (!g_config.quiet) {
            std::cout << "\n=== Generated Code ===" << std::endl;
            std::cout << generatedCode << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}

int transpileProgram(const std::string& source, const std::string& target, const std::string& outputFile) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        JSTranspiler transpiler;
        std::string jsCode = transpiler.transpile(statements);
        
        std::ostringstream output;
        
        if (target == "react-native") {
            // React Native runtime
            output << "// Generated by SynthFlow for React Native\n";
            output << "import React from 'react';\n";
            output << "import { View, Text, Button, Image, TextInput, ScrollView, TouchableOpacity, SafeAreaView } from 'react-native';\n\n";
            output << "// SynthFlow Runtime\n";
            output << "const print = console.log;\n";
            output << "const len = (x) => x.length;\n";
            output << "const str = (x) => String(x);\n";
            output << "const append = (arr, el) => [...arr, el];\n\n";
            
            // Remove duplicate runtime from transpiler output
            std::string codeWithoutRuntime = jsCode;
            size_t userCodePos = codeWithoutRuntime.find("// User Code");
            if (userCodePos != std::string::npos) {
                codeWithoutRuntime = codeWithoutRuntime.substr(userCodePos);
            }
            output << codeWithoutRuntime;
            output << "\nexport default App;\n";
            
        } else if (target == "pwa") {
            // PWA with service worker setup
            output << "// Generated by SynthFlow for PWA\n";
            output << "// Include this in your index.html with <script type=\"module\">\n\n";
            output << jsCode;
            output << "\n\n// Register service worker\n";
            output << "if ('serviceWorker' in navigator) {\n";
            output << "    navigator.serviceWorker.register('/sw.js')\n";
            output << "        .then(reg => console.log('SW registered'))\n";
            output << "        .catch(err => console.log('SW failed', err));\n";
            output << "}\n";
            
        } else {
            // Default: plain JavaScript
            output << jsCode;
        }
        
        std::string finalCode = output.str();
        
        // Output to file or stdout
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (!file.is_open()) {
                logError("Could not open output file: " + outputFile);
                return 1;
            }
            file << finalCode;
            file.close();
            logSuccess("Transpiled to " + outputFile + " (target: " + target + ")");
        } else {
            std::cout << finalCode;
        }
        
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}

// =============================================================================
// Inline Code Execution (-c)
// =============================================================================

int executeInlineCode(const std::string& code) {
    logInfo("Executing inline code...");
    return runProgram(code);
}

// =============================================================================
// Module Execution (-m)
// =============================================================================

int executeModule(const std::string& moduleName) {
    try {
        logInfo("Resolving module: " + moduleName);
        
        ModuleResolver resolver;
        resolver.setProjectRoot(".");
        
        std::string modulePath = resolver.resolveModulePath(moduleName, ".");
        if (modulePath.empty()) {
            logError("Module not found: " + moduleName);
            return 1;
        }
        
        logInfo("Module resolved to: " + modulePath);
        
        std::string source = readFile(modulePath);
        return runProgram(source);
        
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}

// =============================================================================
// REPL Mode
// =============================================================================

int startRepl() {
    std::cout << "SynthFlow REPL v" << SYNTHFLOW_VERSION << std::endl;
    std::cout << "Type :help for commands, :exit to quit" << std::endl;
    std::cout << std::endl;
    
    Interpreter interpreter;  // Persistent interpreter for REPL session
    std::string line;
    std::string multiLine;
    bool inMultiLine = false;
    
    while (true) {
        if (inMultiLine) {
            std::cout << "... " << std::flush;
        } else {
            std::cout << ">>> " << std::flush;
        }
        
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            break;  // EOF
        }
        
        // Handle meta commands
        if (!inMultiLine) {
            if (line == ":exit" || line == ":quit" || line == ":q") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            if (line == ":help" || line == ":h") {
                std::cout << "REPL Commands:" << std::endl;
                std::cout << "  :exit, :quit, :q    Exit REPL" << std::endl;
                std::cout << "  :help, :h           Show this help" << std::endl;
                std::cout << "  :clear, :c          Clear screen" << std::endl;
                std::cout << "  :load <file>        Load and execute a file" << std::endl;
                std::cout << "  :verbose            Toggle verbose mode" << std::endl;
                std::cout << std::endl;
                continue;
            }
            if (line == ":clear" || line == ":c") {
                std::cout << "\033[2J\033[H";  // ANSI clear screen
                continue;
            }
            if (line == ":verbose") {
                g_config.verbose = !g_config.verbose;
                std::cout << "Verbose mode: " << (g_config.verbose ? "ON" : "OFF") << std::endl;
                continue;
            }
            if (line.substr(0, 6) == ":load ") {
                std::string filename = line.substr(6);
                try {
                    std::string source = readFile(filename);
                    Lexer lexer(source);
                    auto tokens = lexer.tokenize();
                    Parser parser(std::move(tokens));
                    auto statements = parser.parse();
                    interpreter.execute(statements);
                    logSuccess("Loaded " + filename);
                } catch (const std::exception& e) {
                    logError(e.what());
                }
                continue;
            }
        }
        
        if (line.empty()) {
            continue;
        }
        
        // Check for multi-line input (ends with backslash or open brace)
        if (!line.empty() && line.back() == '\\') {
            multiLine += line.substr(0, line.length() - 1) + "\n";
            inMultiLine = true;
            continue;
        }
        
        std::string codeToExecute = inMultiLine ? multiLine + line : line;
        inMultiLine = false;
        multiLine.clear();
        
        // Execute line
        try {
            Lexer lexer(codeToExecute);
            auto tokens = lexer.tokenize();
            
            Parser parser(std::move(tokens));
            auto statements = parser.parse();
            
            interpreter.execute(statements);
            
            // Print result if expression
            Value result = interpreter.getLastValue();
            if (!result.isNull()) {
                std::cout << "\033[33m" << result.toString() << "\033[0m" << std::endl;
            }
        } catch (const std::exception& e) {
            logError(e.what());
        }
    }
    
    return 0;
}

// =============================================================================
// Check Command (Type-check only)
// =============================================================================

int checkProgram(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        
        logSuccess("No errors found");
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}

// =============================================================================
// Main Entry Point
// =============================================================================

int main(int argc, char* argv[]) {
    CLI::App app{"SynthFlow Programming Language"};
    app.set_version_flag("-V,--version", SYNTHFLOW_VERSION);
    
    // Global options
    app.add_flag("-v,--verbose", g_config.verbose, "Enable verbose output");
    app.add_flag("-q,--quiet", g_config.quiet, "Suppress non-essential output");
    app.add_flag("-O", g_config.optimizeLevel, "Optimization level (use -O for level 1, -OO for level 2)");
    app.add_flag("-i,--interactive", g_config.interactive, "Enter REPL after execution");
    
    // Inline code execution
    std::string inlineCode;
    app.add_option("-c", inlineCode, "Execute code directly");
    
    // Module execution
    std::string moduleName;
    app.add_option("-m", moduleName, "Run module as script");
    
    // ==========================================================================
    // Subcommand: run
    // ==========================================================================
    auto run_cmd = app.add_subcommand("run", "Execute a SynthFlow program");
    std::string run_file;
    run_cmd->add_option("file", run_file, "Source file to execute")->required();
    
    // ==========================================================================
    // Subcommand: compile
    // ==========================================================================
    auto compile_cmd = app.add_subcommand("compile", "Compile and show analysis");
    std::string compile_file;
    compile_cmd->add_option("file", compile_file, "Source file to compile")->required();
    
    // ==========================================================================
    auto transpile_cmd = app.add_subcommand("transpile", "Convert to JavaScript");
    std::string transpile_file;
    std::string transpile_target = "js";  // Default: plain JavaScript
    std::string transpile_output;
    transpile_cmd->add_option("file", transpile_file, "Source file to transpile")->required();
    transpile_cmd->add_option("-t,--target", transpile_target, "Target platform: js, react-native, pwa");
    transpile_cmd->add_option("-o,--output", transpile_output, "Output file (default: stdout)");
    
    // ==========================================================================
    // Subcommand: check
    // ==========================================================================
    auto check_cmd = app.add_subcommand("check", "Type-check without execution");
    std::string check_file;
    check_cmd->add_option("file", check_file, "Source file to check")->required();
    
    // ==========================================================================
    // Subcommand: repl
    // ==========================================================================
    auto repl_cmd = app.add_subcommand("repl", "Start interactive REPL");
    
    // ==========================================================================
    // Default file argument (for backwards compatibility)
    // ==========================================================================
    std::string default_file;
    app.add_option("file", default_file, "Source file to run (shorthand for 'run')");
    
    // Parse arguments
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }
    
    // ==========================================================================
    // Handle execution based on parsed arguments
    // ==========================================================================
    
    int result = 0;
    
    // Priority 1: Inline code (-c)
    if (!inlineCode.empty()) {
        result = executeInlineCode(inlineCode);
    }
    // Priority 2: Module execution (-m)
    else if (!moduleName.empty()) {
        result = executeModule(moduleName);
    }
    // Priority 3: Subcommands
    else if (*run_cmd) {
        std::string source = readFile(run_file);
        result = runProgram(source);
    }
    else if (*compile_cmd) {
        std::string source = readFile(compile_file);
        result = compileProgram(source);
    }
    else if (*transpile_cmd) {
        std::string source = readFile(transpile_file);
        result = transpileProgram(source, transpile_target, transpile_output);
    }
    else if (*check_cmd) {
        std::string source = readFile(check_file);
        result = checkProgram(source);
    }
    else if (*repl_cmd) {
        result = startRepl();
    }
    // Priority 4: Default file argument
    else if (!default_file.empty()) {
        std::string source = readFile(default_file);
        result = runProgram(source);
    }
    // Priority 5: No arguments = REPL
    else if (argc == 1) {
        result = startRepl();
    }
    
    // If -i flag was set, enter REPL after execution
    if (g_config.interactive && result == 0 && argc > 1) {
        result = startRepl();
    }
    
    return result;
}
