# Utils Directory Reorganization Mapping

This document maps the old file locations to the new functional organization structure.

## File Movement Mapping

### Core Utilities (src/utils/core/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/file_utils.h` | `src/utils/core/file_utils.h` | File system operations |
| `src/utils/file_utils.cpp` | `src/utils/core/file_utils.cpp` | File system operations |
| `src/utils/string_utils.h` | `src/utils/core/string_utils.h` | String manipulation |
| `src/utils/string_utils.cpp` | `src/utils/core/string_utils.cpp` | String manipulation |
| `src/utils/system_utils.h` | `src/utils/core/system_utils.h` | System utilities |
| `src/utils/system_utils.cpp` | `src/utils/core/system_utils.cpp` | System utilities |
| `src/utils/logging_manager.h` | `src/utils/core/logging_manager.h` | Logging management |

### Build System (src/utils/build/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/build_system_config.h` | `src/utils/build/build_system_config.h` | Build system configuration |
| `src/utils/build_system_config.cpp` | `src/utils/build/build_system_config.cpp` | Build system configuration |
| `src/utils/dependency_manager.h` | `src/utils/build/dependency_manager.h` | Dependency management |
| `src/utils/dependency_manager.cpp` | `src/utils/build/dependency_manager.cpp` | Dependency management |
| `src/utils/interactive_dependency_manager.h` | `src/utils/build/interactive_dependency_manager.h` | Interactive dependency management |
| `src/utils/interactive_dependency_manager.cpp` | `src/utils/build/interactive_dependency_manager.cpp` | Interactive dependency management |

### Project Management (src/utils/project/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/project_scaffolding.h` | `src/utils/project/project_scaffolding.h` | Project structure creation |
| `src/utils/project_scaffolding.cpp` | `src/utils/project/project_scaffolding.cpp` | Project structure creation |
| `src/utils/project_validator.h` | `src/utils/project/project_validator.h` | Project validation |
| `src/utils/post_creation_actions.h` | `src/utils/project/post_creation_actions.h` | Post-creation actions |
| `src/utils/post_creation_actions.cpp` | `src/utils/project/post_creation_actions.cpp` | Post-creation actions |

### User Interface (src/utils/ui/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/interactive_menu.h` | `src/utils/ui/interactive_menu.h` | Interactive menus |
| `src/utils/interactive_menu.cpp` | `src/utils/ui/interactive_menu.cpp` | Interactive menus |
| `src/utils/progress_indicator.h` | `src/utils/ui/progress_indicator.h` | Progress visualization |
| `src/utils/progress_indicator.cpp` | `src/utils/ui/progress_indicator.cpp` | Progress visualization |
| `src/utils/terminal_utils.h` | `src/utils/ui/terminal_utils.h` | Terminal styling and interaction |
| `src/utils/terminal_utils.cpp` | `src/utils/ui/terminal_utils.cpp` | Terminal styling and interaction |
| `src/utils/user_experience.h` | `src/utils/ui/user_experience.h` | User experience management |
| `src/utils/user_experience.cpp` | `src/utils/ui/user_experience.cpp` | User experience management |
| `src/utils/table_formatter.h` | `src/utils/ui/table_formatter.h` | Table formatting |
| `src/utils/table_formatter.cpp` | `src/utils/ui/table_formatter.cpp` | Table formatting |

### External Integration (src/utils/external/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/git_utils.h` | `src/utils/external/git_utils.h` | Git operations |
| `src/utils/git_utils.cpp` | `src/utils/external/git_utils.cpp` | Git operations |
| `src/utils/http_client.h` | `src/utils/external/http_client.h` | HTTP client operations |
| `src/utils/http_client.cpp` | `src/utils/external/http_client.cpp` | HTTP client operations |
| `src/utils/framework_integration.h` | `src/utils/external/framework_integration.h` | Framework integration |
| `src/utils/framework_integration.cpp` | `src/utils/external/framework_integration.cpp` | Framework integration |

### System Validation (src/utils/validation/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/msys2_validator.h` | `src/utils/validation/msys2_validator.h` | MSYS2 validation |
| `src/utils/msys2_validator.cpp` | `src/utils/validation/msys2_validator.cpp` | MSYS2 validation |
| `src/utils/edge_case_handler.h` | `src/utils/validation/edge_case_handler.h` | Edge case handling |
| `src/utils/edge_case_handler.cpp` | `src/utils/validation/edge_case_handler.cpp` | Edge case handling |
| `src/utils/context_sensitive_error_system.h` | `src/utils/validation/context_sensitive_error_system.h` | Context-sensitive error handling |
| `src/utils/context_sensitive_error_system.cpp` | `src/utils/validation/context_sensitive_error_system.cpp` | Context-sensitive error handling |

### Security (src/utils/security/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/security_config.h` | `src/utils/security/security_config.h` | Security configuration |
| `src/utils/security_config.cpp` | `src/utils/security/security_config.cpp` | Security configuration |

### Archive (src/utils/archive/)
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/utils/archive_utils.h` | `src/utils/archive/archive_utils.h` | Archive operations |
| `src/utils/archive_utils.cpp` | `src/utils/archive/archive_utils.cpp` | Archive operations |

## CLI Reorganization

### CLI Parser Movement
| Old Location | New Location | Description |
|--------------|--------------|-------------|
| `src/cli/cli_parser.h` | `src/cli/parsing/cli_parser.h` | CLI argument parsing |
| `src/cli/cli_parser.cpp` | `src/cli/parsing/cli_parser.cpp` | CLI argument parsing |

## Include Path Updates Required

### Pattern Replacements
The following include path patterns need to be updated throughout the codebase:

1. **Core utilities:**
   - `#include "../utils/file_utils.h"` → `#include "../utils/core/file_utils.h"`
   - `#include "utils/file_utils.h"` → `#include "utils/core/file_utils.h"`
   - `#include "../utils/string_utils.h"` → `#include "../utils/core/string_utils.h"`
   - `#include "utils/string_utils.h"` → `#include "utils/core/string_utils.h"`
   - `#include "../utils/system_utils.h"` → `#include "../utils/core/system_utils.h"`
   - `#include "utils/system_utils.h"` → `#include "utils/core/system_utils.h"`

2. **Build system utilities:**
   - `#include "../utils/build_system_config.h"` → `#include "../utils/build/build_system_config.h"`
   - `#include "../utils/dependency_manager.h"` → `#include "../utils/build/dependency_manager.h"`
   - `#include "../utils/interactive_dependency_manager.h"` → `#include "../utils/build/interactive_dependency_manager.h"`

3. **Project management utilities:**
   - `#include "../utils/project_scaffolding.h"` → `#include "../utils/project/project_scaffolding.h"`
   - `#include "../utils/project_validator.h"` → `#include "../utils/project/project_validator.h"`
   - `#include "../utils/post_creation_actions.h"` → `#include "../utils/project/post_creation_actions.h"`

4. **UI utilities:**
   - `#include "../utils/terminal_utils.h"` → `#include "../utils/ui/terminal_utils.h"`
   - `#include "utils/terminal_utils.h"` → `#include "utils/ui/terminal_utils.h"`
   - `#include "../utils/progress_indicator.h"` → `#include "../utils/ui/progress_indicator.h"`
   - `#include "utils/progress_indicator.h"` → `#include "utils/ui/progress_indicator.h"`
   - `#include "../utils/interactive_menu.h"` → `#include "../utils/ui/interactive_menu.h"`
   - `#include "../utils/user_experience.h"` → `#include "../utils/ui/user_experience.h"`
   - `#include "../utils/table_formatter.h"` → `#include "../utils/ui/table_formatter.h"`
   - `#include "utils/table_formatter.h"` → `#include "utils/ui/table_formatter.h"`

5. **External integration utilities:**
   - `#include "../utils/git_utils.h"` → `#include "../utils/external/git_utils.h"`
   - `#include "../utils/http_client.h"` → `#include "../utils/external/http_client.h"`
   - `#include "../utils/framework_integration.h"` → `#include "../utils/external/framework_integration.h"`
   - `#include "utils/framework_integration.h"` → `#include "utils/external/framework_integration.h"`

6. **Validation utilities:**
   - `#include "../utils/msys2_validator.h"` → `#include "../utils/validation/msys2_validator.h"`
   - `#include "../utils/edge_case_handler.h"` → `#include "../utils/validation/edge_case_handler.h"`
   - `#include "../utils/context_sensitive_error_system.h"` → `#include "../utils/validation/context_sensitive_error_system.h"`

7. **Security utilities:**
   - `#include "../utils/security_config.h"` → `#include "../utils/security/security_config.h"`

8. **Archive utilities:**
   - `#include "../utils/archive_utils.h"` → `#include "../utils/archive/archive_utils.h"`

9. **CLI parser:**
   - `#include "../utils/terminal_utils.h"` (from cli_parser) → `#include "../utils/ui/terminal_utils.h"`
   - `#include "cli/cli_parser.h"` → `#include "cli/parsing/cli_parser.h"`

## Files Affected by Include Changes

Based on the analysis, the following files need include path updates:
- All template files in `src/templates/`
- All config files in `src/config/`
- All CLI files in `src/cli/`
- Main application file `src/main.cpp`
- All test files in `tests/`
- Documentation generator in `src/documentation/`
- Testing framework manager in `src/testing/`

## Build System Compatibility

The reorganization is compatible with the existing build system because:
- CMake uses `collect_sources` with `RECURSIVE` flag
- All includes are relative to the `src/` directory
- No changes to library structure or dependencies
- Only file locations change, not functionality
