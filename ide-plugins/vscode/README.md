# SynthFlow Language Support for VS Code

This extension provides full language support for the SynthFlow AI-native programming language.

## Features

- **Syntax Highlighting** for all SynthFlow constructs
- **Code Snippets** for rapid development
- **File Icons** for `.sf` files
- **LSP Integration** for IntelliSense (requires `synthflow-lsp.exe`)

## Installation

1. Download the `.vsix` file from Releases
2. In VS Code: `Extensions` → `...` → `Install from VSIX`

Or install from source:
```bash
cd ide-plugins/vscode
npm install && npm run compile
npx vsce package
code --install-extension synthflow-language-support-0.0.2.vsix
```

## Syntax Highlighting

| Category | Keywords |
|----------|----------|
| **Control Flow** | `fn`, `let`, `if`, `else`, `while`, `for`, `return`, `match`, `try`, `catch` |
| **Types** | `Int`, `Float`, `String`, `Bool`, `Tensor`, `Array`, `Map`, `Set` |
| **AI/ML** | `gemini_complete`, `gemini_chat`, `agent`, `model`, `train`, `predict` |
| **Quantum** | `qubit`, `qreg`, `creg`, `hadamard`, `cnot`, `measure`, `pauliX`, `pauliZ` |

## Snippets

| Prefix | Description |
|--------|-------------|
| `fn` | Function declaration |
| `let` | Variable declaration |
| `if` / `ifelse` | Conditionals |
| `for` / `while` | Loops |
| `gemini` | Gemini AI chat |
| `qcircuit` | Quantum circuit template |
| `train` | ML model training |
| `struct` | Struct definition |
| `try` | Try-catch block |

## File Icons

After installing, go to **File > Preferences > File Icon Theme** and select **SynthFlow Icons** to see the purple SF icon for `.sf` files.

## Supported File Extensions

- `.sf` - SynthFlow source files