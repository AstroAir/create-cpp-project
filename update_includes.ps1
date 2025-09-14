# PowerShell script to update all include statements for reorganized utils files

Write-Host "Updating include statements for reorganized utils files..."

# Get all source files
$sourceFiles = Get-ChildItem -Path . -Recurse -Include "*.cpp", "*.h", "*.hpp"

# Define the replacements for each category
$replacements = @{
    # Build system utilities
    '#include\s+"\.\.\/utils\/build_system_config\.h"' = '#include "../utils/build/build_system_config.h"'
    '#include\s+"\.\.\/utils\/dependency_manager\.h"' = '#include "../utils/build/dependency_manager.h"'
    '#include\s+"\.\.\/utils\/interactive_dependency_manager\.h"' = '#include "../utils/build/interactive_dependency_manager.h"'
    
    # Project management utilities
    '#include\s+"\.\.\/utils\/project_scaffolding\.h"' = '#include "../utils/project/project_scaffolding.h"'
    '#include\s+"\.\.\/utils\/project_validator\.h"' = '#include "../utils/project/project_validator.h"'
    '#include\s+"\.\.\/utils\/post_creation_actions\.h"' = '#include "../utils/project/post_creation_actions.h"'
    
    # UI utilities (additional ones)
    '#include\s+"\.\.\/utils\/interactive_menu\.h"' = '#include "../utils/ui/interactive_menu.h"'
    '#include\s+"\.\.\/utils\/user_experience\.h"' = '#include "../utils/ui/user_experience.h"'
    '#include\s+"\.\.\/utils\/table_formatter\.h"' = '#include "../utils/ui/table_formatter.h"'
    '#include\s+"utils\/table_formatter\.h"' = '#include "utils/ui/table_formatter.h"'
    
    # External integration utilities (additional ones)
    '#include\s+"\.\.\/utils\/http_client\.h"' = '#include "../utils/external/http_client.h"'
    
    # Validation utilities
    '#include\s+"\.\.\/utils\/msys2_validator\.h"' = '#include "../utils/validation/msys2_validator.h"'
    '#include\s+"\.\.\/utils\/edge_case_handler\.h"' = '#include "../utils/validation/edge_case_handler.h"'
    '#include\s+"\.\.\/utils\/context_sensitive_error_system\.h"' = '#include "../utils/validation/context_sensitive_error_system.h"'
    
    # Security utilities
    '#include\s+"\.\.\/utils\/security_config\.h"' = '#include "../utils/security/security_config.h"'
    
    # Archive utilities
    '#include\s+"\.\.\/utils\/archive_utils\.h"' = '#include "../utils/archive/archive_utils.h"'
    
    # Test file patterns (with ../../src/ prefix)
    '#include\s+"\.\.\/\.\.\/src\/utils\/file_utils\.h"' = '#include "../../src/utils/core/file_utils.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/string_utils\.h"' = '#include "../../src/utils/core/string_utils.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/terminal_utils\.h"' = '#include "../../src/utils/ui/terminal_utils.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/msys2_validator\.h"' = '#include "../../src/utils/validation/msys2_validator.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/context_sensitive_error_system\.h"' = '#include "../../src/utils/validation/context_sensitive_error_system.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/enhanced_confirmation_system\.h"' = '#include "../../src/utils/validation/enhanced_confirmation_system.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/operation_rollback_system\.h"' = '#include "../../src/utils/validation/operation_rollback_system.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/enhanced_help_system\.h"' = '#include "../../src/utils/validation/enhanced_help_system.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/archive_utils\.h"' = '#include "../../src/utils/archive/archive_utils.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/http_client\.h"' = '#include "../../src/utils/external/http_client.h"'
    '#include\s+"\.\.\/\.\.\/src\/utils\/git_utils\.h"' = '#include "../../src/utils/external/git_utils.h"'
    
    # Special case for src/ prefix (used in some test files)
    '#include\s+"src\/utils\/terminal_utils\.h"' = '#include "src/utils/ui/terminal_utils.h"'
}

# Apply each replacement
foreach ($pattern in $replacements.Keys) {
    $replacement = $replacements[$pattern]
    Write-Host "Updating pattern: $pattern -> $replacement"
    
    foreach ($file in $sourceFiles) {
        try {
            $content = Get-Content $file.FullName -Raw
            if ($content -match $pattern) {
                $newContent = $content -replace $pattern, $replacement
                Set-Content -Path $file.FullName -Value $newContent -NoNewline
                Write-Host "  Updated: $($file.FullName)"
            }
        }
        catch {
            Write-Warning "Error processing file $($file.FullName): $($_.Exception.Message)"
        }
    }
}

Write-Host "Include statement updates completed!"
