# Contributing to SynthFlow

Thank you for your interest in contributing to SynthFlow! This document provides guidelines and information to help you get started.

## Code of Conduct

Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md) to ensure a welcoming environment for all contributors.

## Ways to Contribute

### Reporting Bugs

Before submitting a bug report:
1. Check if the bug has already been reported
2. Try to reproduce the issue with the latest version
3. Gather relevant information (version, OS, steps to reproduce)

When submitting a bug report:
- Use a clear and descriptive title
- Describe the exact steps to reproduce the problem
- Provide specific examples and error messages
- Explain what you expected to happen vs. what actually happened

### Suggesting Enhancements

Feature requests are welcome! Before submitting:
1. Check if the enhancement has already been suggested
2. Consider if it aligns with SynthFlow's design principles
3. Think about implementation complexity

When suggesting enhancements:
- Use a clear and descriptive title
- Provide a detailed description of the proposed feature
- Explain why this enhancement would be useful
- Include examples of how it would be used

### Code Contributions

#### Getting Started
1. Fork the repository
2. Clone your fork
3. Create a new branch for your feature or fix
4. Make your changes
5. Write tests if applicable
6. Update documentation
7. Submit a pull request

#### Development Setup
1. Install required dependencies (see [README.md](../README.md))
2. Clone the repository
3. Build the project using `make all`
4. Run tests using `make test`

#### Pull Request Process
1. Ensure your code follows the style guidelines
2. Update documentation if you've changed functionality
3. Add tests for new features or bug fixes
4. Ensure all tests pass
5. Submit your pull request with a clear description

## Style Guides

### Git Commit Messages
- Use present tense ("Add feature" not "Added feature")
- Use imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit first line to 72 characters
- Reference issues and pull requests liberally

### C++ Code Style
We follow the Google C++ Style Guide with some exceptions:
- Use `std::unique_ptr` for automatic memory management
- Prefer RAII over manual resource management
- Use `const` correctness extensively
- Follow naming conventions:
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Variables: `snake_case`
  - Constants: `kPascalCase`
  - Macros: `UPPER_CASE`

### Documentation Style
- Use Markdown for documentation
- Write in clear, concise English
- Include code examples when appropriate
- Use proper heading hierarchy
- Link to related documents

## Development Workflow

### Branching Strategy
- `main` - Stable releases
- `develop` - Development branch
- `feature/*` - Feature branches
- `hotfix/*` - Hotfix branches
- `release/*` - Release preparation branches

### Issue Tracking
- Use GitHub Issues for bug reports and feature requests
- Label issues appropriately
- Assign yourself to issues you're working on
- Update issue status regularly

### Code Review Process
All changes must be reviewed before merging:
- At least one approval required
- CI checks must pass
- Documentation must be updated
- Tests must pass

## Testing

### Unit Tests
- Located in `compiler/tests/`
- Use Google Test framework
- Test one thing at a time
- Name tests descriptively

### Integration Tests
- Test complete workflows
- Verify compiler output
- Check error handling

### Performance Tests
- Measure compilation speed
- Monitor memory usage
- Benchmark generated code

## Documentation

### Updating Documentation
- Update docs when adding/modifying features
- Keep README.md current
- Document public APIs
- Include examples

### Adding New Documentation
- Place in appropriate directory in `docs/`
- Link from related documents
- Use clear filenames
- Follow documentation style guide

## Community

### Communication Channels
- GitHub Issues for bug reports and feature requests
- GitHub Discussions for general discussion
- Discord/Slack for real-time chat (if available)

### Getting Help
- Check existing documentation
- Search issues and discussions
- Ask in appropriate channels
- Be patient and respectful

## Recognition

### Contributor Listing
Regular contributors will be added to:
- CONTRIBUTORS.md
- Website acknowledgments
- Release notes

### Acknowledgment Policy
- All contributions are acknowledged
- Significant contributions receive special recognition
- Companies supporting development are listed

## Legal

### License
By contributing, you agree that your contributions will be licensed under the MIT License.

### Copyright
- Contributors retain copyright to their contributions
- SynthFlow project holds copyright to the collective work
- Third-party code must be properly attributed

### Patent Grant
Contributors grant a patent license for their contributions.

## Getting Started Checklist

1. [ ] Read this document
2. [ ] Set up development environment
3. [ ] Build the project
4. [ ] Run tests
5. [ ] Find an issue to work on
6. [ ] Fork the repository
7. [ ] Create a feature branch
8. [ ] Make changes
9. [ ] Write tests
10. [ ] Update documentation
11. [ ] Submit pull request

## Need Help?

If you need help getting started:
1. Look for issues labeled "good first issue"
2. Join our community channels
3. Contact maintainers directly
4. Check the documentation
5. Browse existing pull requests

Thank you for contributing to SynthFlow!