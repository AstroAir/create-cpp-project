# Documentation Generator Component

The Documentation Generator component provides automated documentation generation for C++ projects, including README files, API documentation, and project guides.

## Overview

This component handles:

- **Automated README Generation** - Project-specific README files with badges, installation instructions, and usage examples
- **API Documentation** - Doxygen integration for comprehensive API documentation
- **Multiple Output Formats** - HTML, PDF, Markdown, and other documentation formats
- **Template-Based Generation** - Customizable documentation templates
- **Integration Support** - GitHub Pages, GitLab Pages, and other hosting platforms

## Features

### README Generation
- **Project badges** - Build status, version, license badges
- **Installation instructions** - Platform-specific installation guides
- **Usage examples** - Code examples and tutorials
- **Contributing guidelines** - Contribution workflow and standards
- **License information** - License details and attribution

### API Documentation
- **Doxygen integration** - Comprehensive C++ API documentation
- **Code examples** - Inline code examples and usage patterns
- **Cross-references** - Automatic linking between related components
- **Search functionality** - Full-text search across documentation
- **Multiple themes** - Professional documentation themes

### Documentation Formats
- **HTML** - Web-based documentation with navigation
- **PDF** - Printable documentation for offline use
- **Markdown** - GitHub/GitLab compatible documentation
- **LaTeX** - Academic and professional document formatting

## Component Architecture

```cpp
namespace cpp_scaffold::documentation {
    class DocumentationGenerator {
    public:
        bool generate_readme(const ProjectOptions& options);
        bool generate_api_docs(const ProjectOptions& options);
        bool generate_user_guide(const ProjectOptions& options);
        bool setup_documentation_build(const ProjectOptions& options);
    };
    
    class DoxygenIntegration {
    public:
        bool setup_doxygen(const ProjectOptions& options);
        bool generate_doxyfile(const ProjectOptions& options);
        bool build_documentation();
    };
    
    class TemplateEngine {
    public:
        std::string render_template(const std::string& template_name, 
                                  const TemplateContext& context);
        bool register_template(const std::string& name, 
                             const std::string& content);
    };
}
```

## Generated Documentation Structure

### Basic Structure
```
docs/
├── README.md               # Main project documentation
├── INSTALLATION.md         # Installation instructions
├── USAGE.md               # Usage guide and examples
├── API.md                 # API reference
├── CONTRIBUTING.md        # Contributing guidelines
├── CHANGELOG.md           # Version history
├── doxygen/               # Doxygen configuration
│   ├── Doxyfile          # Doxygen configuration file
│   └── custom.css        # Custom styling
└── assets/               # Documentation assets
    ├── images/
    └── diagrams/
```

### Advanced Structure (with MkDocs)
```
docs/
├── index.md              # Documentation homepage
├── getting-started/      # Getting started guides
├── user-guide/          # User documentation
├── developer-guide/     # Developer documentation
├── api-reference/       # API documentation
├── tutorials/           # Step-by-step tutorials
└── assets/             # Documentation assets
mkdocs.yml              # MkDocs configuration
```

## README Template

### Generated README.md
```markdown
# {{project_name}}

{{badges}}

{{description}}

## 🚀 Quick Start

### Prerequisites
{{prerequisites}}

### Installation
{{installation_instructions}}

### Usage
{{usage_examples}}

## 📚 Documentation
{{documentation_links}}

## 🤝 Contributing
{{contributing_info}}

## 📄 License
{{license_info}}
```

## Doxygen Integration

### Doxyfile Configuration
```doxygen
# Project information
PROJECT_NAME           = "{{project_name}}"
PROJECT_NUMBER         = "{{version}}"
PROJECT_BRIEF          = "{{description}}"

# Input configuration
INPUT                  = src/ include/
RECURSIVE              = YES
FILE_PATTERNS          = *.cpp *.h *.hpp

# Output configuration
GENERATE_HTML          = YES
HTML_OUTPUT            = html
GENERATE_LATEX         = NO

# Documentation extraction
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES

# Styling
HTML_EXTRA_STYLESHEET  = doxygen/custom.css
USE_MATHJAX           = YES
```

### Custom CSS Styling
```css
/* Custom Doxygen styling */
.header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
}

.contents {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}

.memitem {
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}
```

## MkDocs Integration

### MkDocs Configuration
```yaml
site_name: {{project_name}} Documentation
site_description: {{description}}

theme:
  name: material
  palette:
    - scheme: default
      primary: blue
      accent: blue

nav:
  - Home: index.md
  - Getting Started: getting-started.md
  - User Guide: user-guide.md
  - API Reference: api-reference.md
  - Contributing: contributing.md

plugins:
  - search
  - mkdocstrings:
      handlers:
        cpp:
          paths: [include]
```

## Build System Integration

### CMake Documentation Target
```cmake
# Find Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
    # Configure Doxyfile
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    
    # Add documentation target
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
```

### GitHub Actions Documentation
```yaml
name: Documentation

on:
  push:
    branches: [ main ]

jobs:
  docs:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Doxygen
      run: sudo apt-get install doxygen graphviz
    
    - name: Generate Documentation
      run: |
        mkdir build
        cd build
        cmake ..
        make docs
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./build/docs/html
```

## Configuration Options

### Documentation Generation
```bash
# Enable documentation generation
cpp-scaffold MyProject --docs

# Specify documentation format
cpp-scaffold MyProject --docs --doc-format doxygen

# Enable MkDocs
cpp-scaffold MyProject --docs --doc-system mkdocs

# Custom documentation theme
cpp-scaffold MyProject --docs --doc-theme material
```

### Advanced Options
```bash
# Generate comprehensive documentation
cpp-scaffold MyProject --docs --api-docs --user-guide --tutorials

# Enable GitHub Pages deployment
cpp-scaffold MyProject --docs --deploy-pages

# Custom documentation directory
cpp-scaffold MyProject --docs --doc-dir documentation
```

## Template Customization

### Custom README Template
```markdown
# {{project_name}}

[![Build Status]({{ci_badge_url}})]({{ci_url}})
[![License]({{license_badge_url}})]({{license_url}})

{{project_description}}

## Features
{{#features}}
- {{.}}
{{/features}}

## Installation
{{installation_section}}

## Usage
{{usage_section}}
```

### Template Variables
- `{{project_name}}` - Project name
- `{{description}}` - Project description
- `{{version}}` - Project version
- `{{author}}` - Author name
- `{{license}}` - License type
- `{{ci_badge_url}}` - CI badge URL
- `{{features}}` - Project features list

## Best Practices

### Documentation Writing
1. **Clear and concise** - Write clear, concise documentation
2. **Code examples** - Include practical code examples
3. **Keep updated** - Maintain documentation with code changes
4. **User-focused** - Write from the user's perspective

### API Documentation
1. **Document public APIs** - Focus on public interfaces
2. **Include examples** - Provide usage examples
3. **Explain parameters** - Document all parameters and return values
4. **Link related functions** - Cross-reference related functionality

### Maintenance
1. **Automated generation** - Use automated tools where possible
2. **Version control** - Track documentation changes
3. **Review process** - Include documentation in code reviews
4. **Regular updates** - Schedule regular documentation reviews

## Related Documentation

- [User Guide - Documentation](../user-guide/index.md)
- [Developer Guide - Contributing](../developer-guide/contributing.md)
- [API Reference](../api-reference/api.md)
