# Command-Line Interface Reference

This document provides a comprehensive reference for the SynthFlow command-line interface (CLI), including all available commands, options, and usage examples.

## Overview

The SynthFlow CLI is the primary tool for interacting with the SynthFlow programming language, providing commands for compilation, package management, testing, and development tasks.

## Basic Usage

```bash
synthflow [OPTIONS] [COMMAND] [ARGS]
```

## Global Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h`, `--help` | Display help information | |
| `-V`, `--version` | Display version information | |
| `-v`, `--verbose` | Enable verbose output | |
| `-q`, `--quiet` | Suppress non-error output | |
| `--color <when>` | Control color output (auto, always, never) | auto |

## Core Commands

### compile
Compile SynthFlow source code to executable or library.

```bash
synthflow compile [OPTIONS] <SOURCE_FILE>
```

#### Options
| Option | Description |
|--------|-------------|
| `-o`, `--output <FILE>` | Specify output file name |
| `--target <TARGET>` | Specify compilation target (x86_64, aarch64, wasm32, etc.) |
| `--release` | Compile with optimizations for release |
| `--debug` | Include debug information |
| `--no-optimize` | Disable optimizations |
| `--emit-llvm` | Emit LLVM IR instead of machine code |
| `--emit-asm` | Emit assembly code |
| `--emit-bc` | Emit LLVM bitcode |

#### Examples
```bash
# Compile a simple program
synthflow compile main.sf

# Compile for release with custom output name
synthflow compile --release -o myapp main.sf

# Compile to WebAssembly
synthflow compile --target wasm32-wasi web_app.sf

# Emit LLVM IR for analysis
synthflow compile --emit-llvm main.sf
```

### run
Compile and run a SynthFlow program.

```bash
synthflow run [OPTIONS] <SOURCE_FILE> [ARGS]...
```

#### Options
| Option | Description |
|--------|-------------|
| `--release` | Run optimized release build |
| `--debug` | Run with debug information |
| `--target <TARGET>` | Specify target platform |
| `--args <ARGS>` | Pass arguments to the program |

#### Examples
```bash
# Run a program
synthflow run main.sf

# Run with program arguments
synthflow run main.sf --input data.txt --output result.txt

# Run optimized version
synthflow run --release main.sf
```

### test
Run tests for SynthFlow projects.

```bash
synthflow test [OPTIONS] [FILTER]
```

#### Options
| Option | Description |
|--------|-------------|
| `--release` | Run tests in release mode |
| `--verbose` | Show output of successful tests |
| `--quiet` | Only show test failures |
| `--coverage` | Generate code coverage report |
| `--filter <PATTERN>` | Run tests matching pattern |
| `--parallel` | Run tests in parallel |
| `--fail-fast` | Stop on first failure |

#### Examples
```bash
# Run all tests
synthflow test

# Run tests with coverage
synthflow test --coverage

# Run specific tests
synthflow test "user_authentication"

# Run tests in parallel
synthflow test --parallel
```

### bench
Run benchmarks for performance testing.

```bash
synthflow bench [OPTIONS] [FILTER]
```

#### Options
| Option | Description |
|--------|-------------|
| `--release` | Run benchmarks in release mode |
| `--verbose` | Show detailed benchmark results |
| `--filter <PATTERN>` | Run benchmarks matching pattern |
| `--compare <BASELINE>` | Compare against baseline results |
| `--save-baseline <NAME>` | Save results as baseline |

#### Examples
```bash
# Run all benchmarks
synthflow bench

# Compare with previous baseline
synthflow bench --compare v1.0

# Save current results as baseline
synthflow bench --save-baseline current
```

### fmt
Format SynthFlow source code.

```bash
synthflow fmt [OPTIONS] [PATH]
```

#### Options
| Option | Description |
|--------|-------------|
| `--check` | Check formatting without modifying files |
| `--verbose` | Show which files are being formatted |
| `--recursive` | Format files recursively in directories |
| `--config <FILE>` | Use specific formatting configuration |

#### Examples
```bash
# Format all files in current directory
synthflow fmt

# Check formatting without changes
synthflow fmt --check

# Format specific file
synthflow fmt src/main.sf
```

### lint
Lint SynthFlow source code for potential issues.

```bash
synthflow lint [OPTIONS] [PATH]
```

#### Options
| Option | Description |
|--------|-------------|
| `--fix` | Automatically fix issues when possible |
| `--verbose` | Show detailed linting results |
| `--config <FILE>` | Use specific linting configuration |
| `--fail-on-warnings` | Treat warnings as errors |

#### Examples
```bash
# Lint current directory
synthflow lint

# Fix issues automatically
synthflow lint --fix

# Fail on warnings
synthflow lint --fail-on-warnings
```

### doc
Generate documentation for SynthFlow projects.

```bash
synthflow doc [OPTIONS] [PATH]
```

#### Options
| Option | Description |
|--------|-------------|
| `--output <DIR>` | Specify output directory |
| `--open` | Open documentation in browser |
| `--format <FORMAT>` | Output format (html, md, json) |
| `--private` | Include private items in documentation |

#### Examples
```bash
# Generate HTML documentation
synthflow doc

# Generate and open in browser
synthflow doc --open

# Generate JSON documentation
synthflow doc --format json
```

## Package Management Commands

### pkg
Manage packages and dependencies.

```bash
synthflow pkg <SUBCOMMAND>
```

#### Subcommands

##### install
Install packages.

```bash
synthflow pkg install [OPTIONS] <PACKAGE>...
```

| Option | Description |
|--------|-------------|
| `--version <VERSION>` | Install specific version |
| `--dev` | Install as development dependency |
| `--global` | Install globally |

##### uninstall
Remove packages.

```bash
synthflow pkg uninstall [OPTIONS] <PACKAGE>...
```

| Option | Description |
|--------|-------------|
| `--global` | Uninstall global package |

##### update
Update packages.

```bash
synthflow pkg update [OPTIONS] [PACKAGE]...
```

| Option | Description |
|--------|-------------|
| `--all` | Update all packages |
| `--dry-run` | Show what would be updated |

##### search
Search for packages.

```bash
synthflow pkg search [OPTIONS] <QUERY>
```

| Option | Description |
|--------|-------------|
| `--limit <N>` | Limit results to N packages |

##### list
List installed packages.

```bash
synthflow pkg list [OPTIONS]
```

| Option | Description |
|--------|-------------|
| `--global` | List globally installed packages |
| `--tree` | Show dependency tree |

##### publish
Publish package to registry.

```bash
synthflow pkg publish [OPTIONS]
```

| Option | Description |
|--------|-------------|
| `--dry-run` | Validate without publishing |
| `--allow-dirty` | Allow publishing with uncommitted changes |

#### Examples
```bash
# Install package
synthflow pkg install web-server

# Install specific version
synthflow pkg install web-server --version 1.2.3

# Update all packages
synthflow pkg update --all

# Search for packages
synthflow pkg search "database"

# List installed packages
synthflow pkg list --tree
```

## Development Tools

### new
Create a new SynthFlow project.

```bash
synthflow new [OPTIONS] <PROJECT_NAME>
```

#### Options
| Option | Description |
|--------|-------------|
| `--bin` | Create a binary application |
| `--lib` | Create a library |
| `--template <TEMPLATE>` | Use specific project template |
| `--vcs <VCS>` | Initialize version control (git, none) |

#### Examples
```bash
# Create binary application
synthflow new my-app

# Create library
synthflow new --lib my-library

# Create with template
synthflow new my-web-app --template web-server
```

### init
Initialize a SynthFlow project in an existing directory.

```bash
synthflow init [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--bin` | Initialize as binary application |
| `--lib` | Initialize as library |
| `--vcs <VCS>` | Initialize version control |

#### Examples
```bash
# Initialize in current directory
synthflow init

# Initialize as library
synthflow init --lib
```

### build
Compile the current project.

```bash
synthflow build [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--release` | Build for release |
| `--target <TARGET>` | Build for specific target |
| `--features <FEATURES>` | Enable specific features |
| `--all-targets` | Build all targets |

#### Examples
```bash
# Build project
synthflow build

# Build for release
synthflow build --release

# Build for specific target
synthflow build --target aarch64-apple-darwin
```

### clean
Remove generated artifacts.

```bash
synthflow clean [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--release` | Clean release artifacts |
| `--doc` | Clean documentation |
| `--all` | Clean all artifacts |

#### Examples
```bash
# Clean build artifacts
synthflow clean

# Clean everything
synthflow clean --all
```

### check
Check project for errors without producing artifacts.

```bash
synthflow check [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--all-targets` | Check all targets |
| `--features <FEATURES>` | Check with specific features |

#### Examples
```bash
# Quick syntax check
synthflow check

# Check all targets
synthflow check --all-targets
```

## Debugging Commands

### debug
Debug a SynthFlow program.

```bash
synthflow debug [OPTIONS] <SOURCE_FILE>
```

#### Options
| Option | Description |
|--------|-------------|
| `--breakpoint <LOCATION>` | Set breakpoint |
| `--args <ARGS>` | Pass arguments to program |
| `--gdb` | Use GDB debugger |

#### Examples
```bash
# Debug program
synthflow debug main.sf

# Debug with arguments
synthflow debug main.sf --args input.txt
```

### profile
Profile a SynthFlow program for performance analysis.

```bash
synthflow profile [OPTIONS] <SOURCE_FILE>
```

#### Options
| Option | Description |
|--------|-------------|
| `--time` | Profile execution time |
| `--memory` | Profile memory usage |
| `--output <FILE>` | Save profile data to file |
| `--format <FORMAT>` | Output format (text, json, flamegraph) |

#### Examples
```bash
# Profile execution time
synthflow profile --time main.sf

# Profile memory usage
synthflow profile --memory main.sf

# Generate flamegraph
synthflow profile --format flamegraph main.sf
```

## Configuration Commands

### config
Manage SynthFlow configuration.

```bash
synthflow config <SUBCOMMAND>
```

#### Subcommands

##### get
Get configuration value.

```bash
synthflow config get <KEY>
```

##### set
Set configuration value.

```bash
synthflow config set <KEY> <VALUE>
```

##### unset
Remove configuration value.

```bash
synthflow config unset <KEY>
```

##### list
List all configuration values.

```bash
synthflow config list
```

#### Examples
```bash
# Get configuration
synthflow config get http.proxy

# Set configuration
synthflow config set http.proxy http://proxy.company.com:8080

# List all configuration
synthflow config list
```

## Self-Management Commands

### self-update
Update the SynthFlow toolchain.

```bash
synthflow self-update [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--version <VERSION>` | Update to specific version |
| `--force` | Force update even if current is newer |
| `--dry-run` | Show what would be updated |

#### Examples
```bash
# Update to latest version
synthflow self-update

# Update to specific version
synthflow self-update --version 1.2.3

# Check what would be updated
synthflow self-update --dry-run
```

### self-uninstall
Uninstall SynthFlow.

```bash
synthflow self-uninstall [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--force` | Skip confirmation prompt |

#### Examples
```bash
# Uninstall SynthFlow
synthflow self-uninstall
```

## Language Server Commands

### lsp
Start the Language Server Protocol server.

```bash
synthflow lsp [OPTIONS]
```

#### Options
| Option | Description |
|--------|-------------|
| `--stdio` | Use stdin/stdout for communication |
| `--port <PORT>` | Listen on specific port |
| `--log-level <LEVEL>` | Set logging level |

#### Examples
```bash
# Start LSP server
synthflow lsp

# Start on specific port
synthflow lsp --port 8080
```

## Environment Variables

SynthFlow recognizes several environment variables:

| Variable | Description |
|----------|-------------|
| `SYNTHFLOW_HOME` | SynthFlow installation directory |
| `SYNTHFLOW_LOG` | Log level (error, warn, info, debug, trace) |
| `SYNTHFLOW_TARGET` | Default compilation target |
| `HTTP_PROXY` | HTTP proxy for network requests |
| `HTTPS_PROXY` | HTTPS proxy for network requests |
| `NO_PROXY` | Hosts to bypass proxy |

## Configuration Files

### synthflow.toml
Project configuration file:

```toml
[package]
name = "my-project"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]
edition = "2025"

[dependencies]
web-server = "1.0"
database = "2.0"

[dev-dependencies]
testing = "1.0"

[build-dependencies]
codegen = "1.0"

[features]
default = ["web-server"]
web-server = []
database = []
```

### synthflow/config.toml
User configuration file:

```toml
[http]
proxy = "http://proxy.company.com:8080"
ssl_verify = true

[build]
jobs = 4
target = "x86_64-unknown-linux-gnu"

[term]
color = "auto"
verbose = false
```

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | General error |
| 101 | Invalid argument |
| 102 | IO error |
| 103 | Compilation error |
| 104 | Test failure |

## Examples

### Complete Workflow
```bash
# Create new project
synthflow new my-web-app
cd my-web-app

# Add dependencies
synthflow pkg install web-server database

# Edit code
# ... edit src/main.sf ...

# Check for errors
synthflow check

# Run tests
synthflow test

# Build release version
synthflow build --release

# Run application
synthflow run --release

# Generate documentation
synthflow doc --open
```

### CI/CD Integration
```bash
# In CI script
synthflow check
synthflow test --fail-fast
synthflow build --release
synthflow pkg publish --dry-run
```

This CLI reference provides comprehensive information about all available SynthFlow commands and options. For the most current information, use `synthflow --help` or `synthflow <command> --help` for specific command details.