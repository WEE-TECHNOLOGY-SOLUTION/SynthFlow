# SynthFlow Language Support for VS Code

This extension provides syntax highlighting for the SynthFlow programming language.

## Features

- Syntax highlighting for SynthFlow keywords
- String literal highlighting
- Comment highlighting
- Number highlighting
- Operator highlighting

## Installation

1. Clone this repository
2. Open VS Code
3. Run `code --install-extension .` in the vscode directory

Or copy the entire vscode directory to your VS Code extensions folder:

- Windows: `%USERPROFILE%\.vscode\extensions`
- macOS: `~/.vscode/extensions`
- Linux: `~/.vscode/extensions`

## Supported File Extensions

- `.sf` - SynthFlow source files

## Syntax Highlighting

The extension provides colorization for:

- Keywords: `fn`, `let`, `if`, `else`, `while`, `return`, `type`, `struct`, `enum`, `intent`, `true`, `false`
- Comments: `# This is a comment`
- Strings: `"Hello, World!"`
- Numbers: `42`, `3.14`
- Operators: `+`, `-`, `*`, `/`, `%`, `=`, `!`, `<`, `>`, `&`, `|`