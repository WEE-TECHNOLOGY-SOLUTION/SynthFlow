# Contributing to SynthFlow

Thank you for your interest in contributing to SynthFlow! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/your-username/synthflow.git`
3. Create a new branch: `git checkout -b feature/your-feature-name`
4. Make your changes
5. Commit your changes: `git commit -m "Add your feature description"`
6. Push to your fork: `git push origin feature/your-feature-name`
7. Create a Pull Request

## Development Setup

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- GNU Make
- Git

### Building

```bash
# Navigate to the project directory
cd synthflow

# Build the project
make all

# Run tests
make test
```

## Code Style

- Follow the existing code style in the project
- Use consistent indentation (4 spaces)
- Write clear, descriptive variable and function names
- Add comments for complex logic
- Keep functions focused on a single responsibility

## Testing

- Add tests for new features
- Ensure all existing tests pass before submitting a PR
- Write clear test names that describe what is being tested

## Pull Request Process

1. Ensure your code follows the project's coding standards
2. Add tests for any new functionality
3. Update documentation if necessary
4. Describe your changes in the PR description
5. Reference any related issues

## Reporting Issues

- Use the GitHub issue tracker
- Describe the issue clearly
- Include steps to reproduce
- Provide information about your environment (OS, compiler version, etc.)

## Code of Conduct

Please be respectful and constructive in all interactions. We welcome contributions from everyone.