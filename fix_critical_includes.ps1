#!/usr/bin/env pwsh

# Critical Include Path Fix Script
Write-Host "Fixing critical include path issues..." -ForegroundColor Green

# Define critical include path fixes
$includeFixes = @{
    '"../../utils/terminal_utils.h"' = '"../utils/ui/terminal_utils.h"'
    '"../../utils/context_sensitive_error_system.h"' = '"../utils/validation/context_sensitive_error_system.h"'
    '"../../utils/string_utils.h"' = '"../utils/core/string_utils.h"'
    '"../cli/cli_parser.h"' = '"../../cli/cli_parser.h"'  # For files in utils subdirectories
}

# Get all source files
$sourceFiles = Get-ChildItem -Path "src" -Recurse -Include "*.cpp", "*.h", "*.hpp"

foreach ($file in $sourceFiles) {
    try {
        $content = Get-Content -Path $file.FullName -Raw -Encoding UTF8
        $originalContent = $content
        
        # Apply include fixes
        foreach ($fix in $includeFixes.GetEnumerator()) {
            $content = $content -replace [regex]::Escape($fix.Key), $fix.Value
        }
        
        # Only write if content changed
        if ($content -ne $originalContent) {
            Set-Content -Path $file.FullName -Value $content -Encoding UTF8 -NoNewline
            Write-Host "  Fixed includes in: $($file.Name)" -ForegroundColor Green
        }
    }
    catch {
        Write-Host "  Error processing $($file.Name): $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host "Critical include path fixes completed!" -ForegroundColor Green
