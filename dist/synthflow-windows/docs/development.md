This document describes the build system and development environment for SynthFlow.

## Build System

SynthFlow uses a Makefile-based build system for simplicity and portability.

### Prerequisites

- GNU Make 3.81 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Standard C++ library

### Building

To build SynthFlow:

```bash
./scripts/build.sh
```

This will produce:
- `synthflow` - The main compiler executable
- `test_lexer` - Lexer test executable

### Testing

To run tests:

```bash
./scripts/build.sh test
```

### Cleaning

To clean build artifacts:

```bash
./scripts/build.sh clean
```

## Directory Structure

```
synthflow/
├── compiler/           # Compiler source code
│   ├── include/        # Header files
│   ├── src/            # Source files
│   │   ├── lexer/      # Lexer implementation
│   │   └── main.cpp    # Main entry point
│   └── tests/          # Test files
├── runtime/            # Runtime library
├── stdlib/             # Standard library
├── lsp-server/         # Language Server Protocol implementation
├── ide-plugins/        # IDE integrations
├── examples/           # Example programs
├── docs/               # Documentation
├── .vscode/            # VS Code configuration
├── Makefile                   # Build configuration
├── scripts/build.sh           # Build script for Unix-like systems
├── scripts/build.bat          # Build script for Windows
└── README.md           # Project overview
```

## Development Environment

### Visual Studio Code

The project includes configuration files for Visual Studio Code:

- `.vscode/tasks.json` - Build tasks
- `.vscode/launch.json` - Debug configurations
- `.vscode/c_cpp_properties.json` - IntelliSense configuration
- `.vscode/settings.json` - Workspace settings

#### Recommended Extensions

- C/C++ (Microsoft)
- Code Runner (Jun Han)
- GitLens (Eric Amodio)
- Bracket Pair Colorizer (CoenraadS)

### IDE Plugins

#### VS Code Extension

Located in `ide-plugins/vscode/`, provides:
- Syntax highlighting
- File associations
- Basic code folding

#### IntelliJ IDEA Plugin (Planned)

Will be located in `ide-plugins/jetbrains/`.

## Version Control

The project uses Git for version control.

### Branching Strategy

- `main` - Stable releases
- `develop` - Development branch
- `feature/*` - Feature branches
- `hotfix/*` - Hotfix branches

### Commit Guidelines

- Use present tense ("Add feature" not "Added feature")
- Use imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit first line to 72 characters
- Reference issues and pull requests liberally

## Coding Standards

### C++ Standards

- C++17 features are allowed
- Follow Google C++ Style Guide
- Use `std::unique_ptr` for automatic memory management
- Prefer RAII over manual resource management

### Naming Conventions

- Classes: `PascalCase`
- Functions: `camelCase`
- Variables: `snake_case`
- Constants: `kPascalCase`
- Macros: `UPPER_CASE`

### File Organization

- Header files: `.h`
- Source files: `.cpp`
- One class per header file when possible
- Inline functions in headers only when necessary

## Testing

### Unit Tests

Located in `compiler/tests/` directory.

#### Test Framework

- Google Test framework
- Each test file focuses on one component
- Tests should be fast and deterministic

#### Running Tests

```bash
./scripts/build.sh test
```

### Continuous Integration

Planned CI setup will:
- Build on multiple platforms
- Run all tests
- Check code formatting
- Verify documentation

## Documentation

Documentation is written in Markdown and located in the `docs/` directory.

### Documentation Style

- Use clear, concise language
- Include code examples
- Provide links to related documents
- Update documentation with code changes

### Generated Documentation

Planned features:
- API documentation from source code
- Tutorial generation
- Example program documentation

## Release Process

### Versioning

SynthFlow follows Semantic Versioning 2.0.0:
- MAJOR version for incompatible changes
- MINOR version for feature additions
- PATCH version for bug fixes

### Release Steps

1. Update version number in relevant files
2. Update CHANGELOG.md
3. Create release branch
4. Run full test suite
5. Create Git tag
6. Build release binaries
7. Publish release

## Contributing

### Getting Started

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Update documentation
6. Submit a pull request

### Code Review Process

All changes must be reviewed before merging:
- At least one approval required
- CI checks must pass
- Documentation must be updated
- Tests must pass

### Reporting Issues

Use GitHub Issues for:
- Bug reports
- Feature requests
- Documentation issues
- Questions

When reporting bugs, include:
- Version information
- Steps to reproduce
- Expected behavior
- Actual behavior
- System information