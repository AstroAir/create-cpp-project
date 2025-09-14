# PowerShell script to fix all include paths after reorganization

Write-Host "Fixing all include paths for reorganized files..."

# Get all source files
$sourceFiles = Get-ChildItem -Path . -Recurse -Include "*.cpp", "*.h", "*.hpp"

# Define the replacements for each category
$replacements = @{
    # Fix config files trying to include cli_parser.h with wrong path
    '#include\s+"\.\.\/\.\.\/cli\/cli_parser\.h"' = '#include "../cli/cli_parser.h"'
    
    # Fix utils files trying to include other utils with wrong paths
    '#include\s+"terminal_utils\.h"' = '#include "../ui/terminal_utils.h"'
    '#include\s+"string_utils\.h"' = '#include "../core/string_utils.h"'
    '#include\s+"file_utils\.h"' = '#include "../core/file_utils.h"'
    '#include\s+"system_utils\.h"' = '#include "../core/system_utils.h"'
    '#include\s+"logging_manager\.h"' = '#include "../core/logging_manager.h"'
    
    # Fix build system utilities
    '#include\s+"dependency_manager\.h"' = '#include "../build/dependency_manager.h"'
    '#include\s+"build_system_config\.h"' = '#include "../build/build_system_config.h"'
    '#include\s+"interactive_dependency_manager\.h"' = '#include "../build/interactive_dependency_manager.h"'
    
    # Fix project management utilities
    '#include\s+"project_scaffolding\.h"' = '#include "../project/project_scaffolding.h"'
    '#include\s+"project_validator\.h"' = '#include "../project/project_validator.h"'
    '#include\s+"post_creation_actions\.h"' = '#include "../project/post_creation_actions.h"'
    
    # Fix UI utilities
    '#include\s+"interactive_menu\.h"' = '#include "../ui/interactive_menu.h"'
    '#include\s+"user_experience\.h"' = '#include "../ui/user_experience.h"'
    '#include\s+"table_formatter\.h"' = '#include "../ui/table_formatter.h"'
    '#include\s+"progress_indicator\.h"' = '#include "../ui/progress_indicator.h"'
    
    # Fix external integration utilities
    '#include\s+"http_client\.h"' = '#include "../external/http_client.h"'
    '#include\s+"git_utils\.h"' = '#include "../external/git_utils.h"'
    '#include\s+"framework_integration\.h"' = '#include "../external/framework_integration.h"'
    
    # Fix validation utilities
    '#include\s+"msys2_validator\.h"' = '#include "../validation/msys2_validator.h"'
    '#include\s+"edge_case_handler\.h"' = '#include "../validation/edge_case_handler.h"'
    '#include\s+"context_sensitive_error_system\.h"' = '#include "../validation/context_sensitive_error_system.h"'
    
    # Fix security utilities
    '#include\s+"security_config\.h"' = '#include "../security/security_config.h"'
    
    # Fix archive utilities
    '#include\s+"archive_utils\.h"' = '#include "../archive/archive_utils.h"'
    
    # Fix CLI includes from utils
    '#include\s+"\.\.\/cli\/input\/user_input\.h"' = '#include "../../cli/input/user_input.h"'
    '#include\s+"\.\.\/cli\/types\/cli_enums\.h"' = '#include "../../cli/types/cli_enums.h"'
    '#include\s+"\.\.\/cli\/cli_parser\.h"' = '#include "../../cli/cli_parser.h"'
    
    # Fix relative paths from different directories
    '#include\s+"\.\.\/utils\/terminal_utils\.h"' = '#include "../utils/ui/terminal_utils.h"'
    '#include\s+"\.\.\/utils\/file_utils\.h"' = '#include "../utils/core/file_utils.h"'
    '#include\s+"\.\.\/utils\/string_utils\.h"' = '#include "../utils/core/string_utils.h"'
    '#include\s+"\.\.\/utils\/system_utils\.h"' = '#include "../utils/core/system_utils.h"'
    
    # Fix includes from templates directory
    '#include\s+"\.\.\/utils\/archive_utils\.h"' = '#include "../utils/archive/archive_utils.h"'
    '#include\s+"\.\.\/utils\/http_client\.h"' = '#include "../utils/external/http_client.h"'
    '#include\s+"\.\.\/utils\/security_config\.h"' = '#include "../utils/security/security_config.h"'
    '#include\s+"\.\.\/utils\/edge_case_handler\.h"' = '#include "../utils/validation/edge_case_handler.h"'
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

Write-Host "Include path updates completed!"
