# Security Policy

## Supported Versions

We take security seriously and provide security updates for the following versions of CPP-Scaffold:

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

We appreciate your efforts to responsibly disclose security vulnerabilities. Please follow these guidelines when reporting security issues:

### How to Report

**Please do NOT report security vulnerabilities through public GitHub issues.**

Instead, please report security vulnerabilities by emailing us at:
- **Email**: security@cpp-scaffold.org (or create this email)
- **Subject**: [SECURITY] Brief description of the vulnerability

### What to Include

When reporting a vulnerability, please include the following information:

1. **Description**: A clear description of the vulnerability
2. **Steps to Reproduce**: Detailed steps to reproduce the issue
3. **Impact**: Description of the potential impact and attack scenarios
4. **Affected Versions**: Which versions of CPP-Scaffold are affected
5. **Environment**: Operating system, compiler, and other relevant environment details
6. **Proof of Concept**: If possible, include a minimal proof of concept
7. **Suggested Fix**: If you have ideas for how to fix the issue

### Response Timeline

We are committed to responding to security reports promptly:

- **Initial Response**: Within 48 hours of receiving your report
- **Status Update**: Within 7 days with our assessment and planned timeline
- **Resolution**: We aim to resolve critical vulnerabilities within 30 days

### Disclosure Policy

- We follow responsible disclosure practices
- We will work with you to understand and resolve the issue
- We will credit you in our security advisory (unless you prefer to remain anonymous)
- We ask that you do not publicly disclose the vulnerability until we have had a chance to address it

### Security Updates

Security updates will be:
- Released as patch versions (e.g., 1.2.3 → 1.2.4)
- Documented in our [CHANGELOG.md](CHANGELOG.md)
- Announced in our GitHub releases
- Tagged with the `security` label

### Security Best Practices

When using CPP-Scaffold, we recommend:

1. **Keep Updated**: Always use the latest version
2. **Validate Input**: Validate all user inputs in generated projects
3. **Secure Dependencies**: Keep dependencies updated in generated projects
4. **Code Review**: Review generated code before using in production
5. **Static Analysis**: Use static analysis tools on generated projects

### Scope

This security policy covers:
- The CPP-Scaffold tool itself
- Generated project templates
- Build scripts and configuration files
- Documentation and examples

This policy does NOT cover:
- Third-party dependencies (report to their respective maintainers)
- User-generated content or modifications
- Issues in projects created with CPP-Scaffold (unless caused by the tool itself)

### Contact

For security-related questions or concerns:
- **Security Email**: security@cpp-scaffold.org
- **General Issues**: [GitHub Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)
- **Discussions**: [GitHub Discussions](https://github.com/cpp-scaffold/cpp-scaffold/discussions)

Thank you for helping keep CPP-Scaffold and our community safe!
