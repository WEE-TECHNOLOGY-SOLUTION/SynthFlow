# IDE Integration Documentation

SynthFlow provides IDE integration through the Language Server Protocol (LSP) and custom extensions for popular IDEs.

## Supported IDEs

1. Visual Studio Code
2. IntelliJ IDEA (planned)
3. Vim/Neovim (planned)

## Visual Studio Code Extension

The VS Code extension provides basic syntax highlighting for SynthFlow files.

### Features

- Syntax highlighting for keywords, strings, comments, and operators
- File association for `.sf` files
- Basic code folding

### Installation

To install the extension:

1. Navigate to the `ide-plugins/vscode` directory
2. Run `npm install` (if package.json is present) or copy the directory to your VS Code extensions folder
3. Restart VS Code

### Configuration

The extension uses the following file associations:
- `.sf` - SynthFlow source files

## Language Server Protocol (Planned)

The LSP implementation will provide advanced IDE features:

- Syntax highlighting
- Code completion
- Error reporting
- Go to definition
- Find references
- Rename symbol
- Hover information

### LSP Features Roadmap

1. Basic syntax highlighting
2. Error reporting with diagnostics
3. Code completion
4. Go to definition
5. Find all references
6. Rename symbol
7. Hover information
8. Signature help
9. Document symbols
10. Workspace symbols

## Future IDE Integrations

### IntelliJ IDEA Plugin

Planned features:
- Syntax highlighting
- Code completion
- Refactoring support
- Debugging integration

### Vim/Neovim Plugin

Planned features:
- Syntax highlighting
- Code completion (with coc.nvim or similar)
- LSP integration