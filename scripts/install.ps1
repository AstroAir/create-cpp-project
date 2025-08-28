#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Universal installation script for cpp-scaffold

.DESCRIPTION
    Cross-platform PowerShell installation script that supports multiple
    installation methods and provides both CLI and GUI modes.

.PARAMETER InstallDir
    Installation directory

.PARAMETER Method
    Installation method (auto, package, binary, source)

.PARAMETER Force
    Force installation (overwrite existing)

.PARAMETER GUI
    Use GUI mode (if available)

.PARAMETER Verbose
    Enable verbose output

.PARAMETER DryRun
    Show what would be done without executing

.PARAMETER Uninstall
    Uninstall cpp-scaffold

.EXAMPLE
    .\install.ps1

.EXAMPLE
    .\install.ps1 -InstallDir "C:\Tools" -Method binary

.EXAMPLE
    .\install.ps1 -GUI -Verbose
#>

param(
    [Parameter()]
    [string]$InstallDir,
    
    [Parameter()]
    [ValidateSet("auto", "package", "binary", "source")]
    [string]$Method = "auto",
    
    [Parameter()]
    [switch]$Force,
    
    [Parameter()]
    [switch]$GUI,
    
    [Parameter()]
    [switch]$Verbose,
    
    [Parameter()]
    [switch]$DryRun,
    
    [Parameter()]
    [switch]$Uninstall
)

# Script metadata
$ScriptVersion = "1.0.0"
$ProjectName = "cpp-scaffold"
$ProjectVersion = "1.0.0"
$GitHubRepo = "cpp-scaffold/cpp-scaffold"

# Set error action preference
$ErrorActionPreference = "Stop"

# Platform detection
$Platform = if ($IsWindows) { "windows" } elseif ($IsMacOS) { "macos" } else { "linux" }
$Arch = if ([Environment]::Is64BitOperatingSystem) { "x86_64" } else { "i386" }

# Default installation paths
$DefaultInstallPrefix = if ($IsWindows) { "$env:ProgramFiles\cpp-scaffold" } else { "/usr/local" }
$UserInstallPrefix = if ($IsWindows) { "$env:LOCALAPPDATA\cpp-scaffold" } else { "$env:HOME/.local" }

# Helper functions
function Write-Banner {
    Write-Host "╔══════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║                    CPP-Scaffold Installer                   ║" -ForegroundColor Cyan
    Write-Host "║              Modern C++ Project Scaffolding Tool            ║" -ForegroundColor Cyan
    Write-Host "╚══════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
}

function Write-Status {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Cyan
}

function Write-Warning {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

function Write-Step {
    param([string]$Message)
    Write-Host "→ $Message" -ForegroundColor Blue
}

# Detect package manager
function Get-PackageManager {
    if (Get-Command choco -ErrorAction SilentlyContinue) {
        return "chocolatey"
    } elseif (Get-Command winget -ErrorAction SilentlyContinue) {
        return "winget"
    } elseif (Get-Command scoop -ErrorAction SilentlyContinue) {
        return "scoop"
    } elseif (Get-Command brew -ErrorAction SilentlyContinue) {
        return "brew"
    } else {
        return "none"
    }
}

# Check if running as administrator
function Test-Administrator {
    if ($IsWindows) {
        $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
        $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
        return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
    } else {
        return (id -u) -eq 0
    }
}

# Get installation directory
function Get-InstallDirectory {
    if ($InstallDir) {
        return $InstallDir
    }
    
    if (Test-Administrator) {
        return $DefaultInstallPrefix
    } else {
        return $UserInstallPrefix
    }
}

# Check if GUI is available
function Test-GUIAvailable {
    if ($IsWindows) {
        return $true  # Windows always has GUI capabilities
    } elseif ($IsMacOS) {
        return (Get-Command osascript -ErrorAction SilentlyContinue) -ne $null
    } else {
        return (Get-Command zenity -ErrorAction SilentlyContinue) -ne $null -or 
               (Get-Command dialog -ErrorAction SilentlyContinue) -ne $null
    }
}

# Show GUI dialog
function Show-GUIDialog {
    param(
        [string]$Title,
        [string]$Message,
        [string]$Type = "Information"
    )
    
    if ($IsWindows) {
        Add-Type -AssemblyName System.Windows.Forms
        $icon = switch ($Type) {
            "Error" { [System.Windows.Forms.MessageBoxIcon]::Error }
            "Warning" { [System.Windows.Forms.MessageBoxIcon]::Warning }
            "Question" { [System.Windows.Forms.MessageBoxIcon]::Question }
            default { [System.Windows.Forms.MessageBoxIcon]::Information }
        }
        
        $buttons = if ($Type -eq "Question") { 
            [System.Windows.Forms.MessageBoxButtons]::YesNo 
        } else { 
            [System.Windows.Forms.MessageBoxButtons]::OK 
        }
        
        $result = [System.Windows.Forms.MessageBox]::Show($Message, $Title, $buttons, $icon)
        return $result -eq [System.Windows.Forms.DialogResult]::Yes -or $result -eq [System.Windows.Forms.DialogResult]::OK
    } elseif ($IsMacOS) {
        $iconType = switch ($Type) {
            "Error" { "stop" }
            "Warning" { "caution" }
            default { "note" }
        }
        
        $script = "display dialog `"$Message`" with title `"$Title`" with icon $iconType"
        if ($Type -eq "Question") {
            $script += " buttons {`"No`", `"Yes`"} default button `"Yes`""
        }
        
        try {
            $result = & osascript -e $script 2>$null
            return $result -match "Yes" -or $Type -ne "Question"
        } catch {
            return $false
        }
    } else {
        # Linux - use zenity or dialog
        if (Get-Command zenity -ErrorAction SilentlyContinue) {
            $zenityType = switch ($Type) {
                "Error" { "--error" }
                "Warning" { "--warning" }
                "Question" { "--question" }
                default { "--info" }
            }
            
            try {
                & zenity $zenityType --title $Title --text $Message
                return $LASTEXITCODE -eq 0
            } catch {
                return $false
            }
        }
        return $false
    }
}

# Download file
function Get-FileFromUrl {
    param(
        [string]$Url,
        [string]$OutputPath
    )
    
    if ($Verbose) {
        Write-Step "Downloading: $Url"
    }
    
    try {
        if ($PSVersionTable.PSVersion.Major -ge 3) {
            Invoke-WebRequest -Uri $Url -OutFile $OutputPath -UseBasicParsing
        } else {
            $webClient = New-Object System.Net.WebClient
            $webClient.DownloadFile($Url, $OutputPath)
        }
        return $true
    } catch {
        Write-Error "Failed to download file: $($_.Exception.Message)"
        return $false
    }
}

# Install via package manager
function Install-ViaPackageManager {
    $packageManager = Get-PackageManager
    Write-Step "Installing via package manager ($packageManager)..."
    
    if ($DryRun) {
        Write-Info "Would install cpp-scaffold using $packageManager"
        return $true
    }
    
    try {
        switch ($packageManager) {
            "chocolatey" {
                & choco install cpp-scaffold -y
            }
            "winget" {
                & winget install cpp-scaffold
            }
            "scoop" {
                & scoop install cpp-scaffold
            }
            "brew" {
                & brew install cpp-scaffold
            }
            default {
                Write-Warning "Package manager installation not supported for $packageManager"
                return $false
            }
        }
        return $LASTEXITCODE -eq 0
    } catch {
        Write-Error "Package manager installation failed: $($_.Exception.Message)"
        return $false
    }
}

# Install from binary
function Install-FromBinary {
    Write-Step "Installing from binary..."
    
    $installDir = Get-InstallDirectory
    $binaryUrl = "https://github.com/$GitHubRepo/releases/latest/download/cpp-scaffold-$ProjectVersion-$Platform-$Arch"
    
    if ($IsWindows) {
        $binaryUrl += ".zip"
    } else {
        $binaryUrl += ".tar.gz"
    }
    
    $tempDir = New-TemporaryFile | ForEach-Object { Remove-Item $_; New-Item -ItemType Directory -Path $_ }
    $archiveFile = Join-Path $tempDir.FullName "cpp-scaffold-archive"
    
    if ($DryRun) {
        Write-Info "Would download: $binaryUrl"
        Write-Info "Would extract to: $installDir"
        Remove-Item -Recurse -Force $tempDir
        return $true
    }
    
    try {
        # Download binary archive
        if (-not (Get-FileFromUrl $binaryUrl $archiveFile)) {
            throw "Failed to download binary"
        }
        
        # Extract archive
        Write-Step "Extracting archive..."
        if ($IsWindows) {
            Expand-Archive -Path $archiveFile -DestinationPath $tempDir.FullName
        } else {
            & tar -xzf $archiveFile -C $tempDir.FullName
        }
        
        # Install files
        Write-Step "Installing to $installDir..."
        
        # Create directories
        $binDir = Join-Path $installDir "bin"
        $shareDir = Join-Path $installDir "share\cpp-scaffold"
        New-Item -ItemType Directory -Path $binDir -Force | Out-Null
        New-Item -ItemType Directory -Path $shareDir -Force | Out-Null
        
        # Copy executable
        $executableName = if ($IsWindows) { "cpp-scaffold.exe" } else { "cpp-scaffold" }
        $sourcePath = Join-Path $tempDir.FullName $executableName
        $targetPath = Join-Path $binDir $executableName
        
        if (Test-Path $sourcePath) {
            Copy-Item $sourcePath $targetPath -Force
        } else {
            throw "Executable not found in archive"
        }
        
        # Copy documentation
        $shareSource = Join-Path $tempDir.FullName "share"
        if (Test-Path $shareSource) {
            Copy-Item -Recurse "$shareSource\*" $shareDir -Force
        }
        
        return $true
    } catch {
        Write-Error "Binary installation failed: $($_.Exception.Message)"
        return $false
    } finally {
        Remove-Item -Recurse -Force $tempDir -ErrorAction SilentlyContinue
    }
}

# Install from source
function Install-FromSource {
    Write-Step "Installing from source..."
    
    $installDir = Get-InstallDirectory
    
    # Check build dependencies
    $buildDeps = @("cmake", "ninja", "git")
    $missingDeps = @()
    
    foreach ($dep in $buildDeps) {
        if (-not (Get-Command $dep -ErrorAction SilentlyContinue)) {
            $missingDeps += $dep
        }
    }
    
    if ($missingDeps.Count -gt 0) {
        Write-Error "Missing build dependencies: $($missingDeps -join ', ')"
        return $false
    }
    
    $tempDir = New-TemporaryFile | ForEach-Object { Remove-Item $_; New-Item -ItemType Directory -Path $_ }
    
    if ($DryRun) {
        Write-Info "Would clone repository to: $($tempDir.FullName)"
        Write-Info "Would build and install to: $installDir"
        Remove-Item -Recurse -Force $tempDir
        return $true
    }
    
    try {
        # Clone repository
        Write-Step "Cloning repository..."
        & git clone "https://github.com/$GitHubRepo.git" $tempDir.FullName
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to clone repository"
        }
        
        Push-Location $tempDir.FullName
        
        # Configure build
        Write-Step "Configuring build..."
        & cmake --preset release
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to configure build"
        }
        
        # Build
        Write-Step "Building..."
        & cmake --build --preset release
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to build"
        }
        
        # Install
        Write-Step "Installing to $installDir..."
        & cmake --install build --prefix $installDir
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to install"
        }
        
        return $true
    } catch {
        Write-Error "Source installation failed: $($_.Exception.Message)"
        return $false
    } finally {
        Pop-Location -ErrorAction SilentlyContinue
        Remove-Item -Recurse -Force $tempDir -ErrorAction SilentlyContinue
    }
}

# Verify installation
function Test-Installation {
    $installDir = Get-InstallDirectory
    $executableName = if ($IsWindows) { "cpp-scaffold.exe" } else { "cpp-scaffold" }
    $executable = Join-Path $installDir "bin\$executableName"
    
    if (-not (Test-Path $executable)) {
        Write-Error "Installation verification failed: executable not found"
        return $false
    }
    
    # Test executable
    try {
        & $executable --version | Out-Null
        Write-Status "Installation verified successfully"
        return $true
    } catch {
        Write-Warning "Installation verification: executable test failed (may be expected)"
        return $true
    }
}

# Update PATH
function Update-Path {
    $installDir = Get-InstallDirectory
    $binDir = Join-Path $installDir "bin"
    
    if ($DryRun) {
        Write-Info "Would add $binDir to PATH"
        return
    }
    
    if ($IsWindows) {
        # Windows PATH update
        $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
        if ($currentPath -notlike "*$binDir*") {
            $newPath = "$binDir;$currentPath"
            [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
            Write-Status "Added $binDir to user PATH"
            Write-Info "Please restart your shell to use cpp-scaffold"
        } else {
            Write-Info "Installation directory already in PATH"
        }
    } else {
        # Unix PATH update
        $shellConfig = if ($env:SHELL -like "*zsh*") { "$env:HOME/.zshrc" } else { "$env:HOME/.bashrc" }
        
        if (Test-Path $shellConfig) {
            $content = Get-Content $shellConfig -Raw
            if ($content -notlike "*$binDir*") {
                Add-Content $shellConfig "`n# Added by cpp-scaffold installer"
                Add-Content $shellConfig "export PATH=`"$binDir:`$PATH`""
                Write-Status "Added $binDir to PATH in $shellConfig"
                Write-Info "Please restart your shell or run: source $shellConfig"
            } else {
                Write-Info "Installation directory already in PATH"
            }
        }
    }
}

# Uninstall
function Uninstall-CppScaffold {
    Write-Step "Uninstalling cpp-scaffold..."
    
    $installDir = Get-InstallDirectory
    $executableName = if ($IsWindows) { "cpp-scaffold.exe" } else { "cpp-scaffold" }
    $executable = Join-Path $installDir "bin\$executableName"
    $shareDir = Join-Path $installDir "share\cpp-scaffold"
    
    if ($DryRun) {
        Write-Info "Would remove: $executable"
        Write-Info "Would remove: $shareDir"
        return
    }
    
    # Remove executable
    if (Test-Path $executable) {
        Remove-Item $executable -Force
        Write-Status "Removed executable"
    }
    
    # Remove documentation
    if (Test-Path $shareDir) {
        Remove-Item -Recurse -Force $shareDir
        Write-Status "Removed documentation"
    }
    
    Write-Status "Uninstallation completed"
}

# Main installation logic
function Invoke-MainInstall {
    if ($Method -eq "auto") {
        $packageManager = Get-PackageManager
        if ($packageManager -ne "none") {
            $Method = "package"
        } else {
            $Method = "binary"
        }
    }
    
    switch ($Method) {
        "package" {
            if (-not (Install-ViaPackageManager)) {
                Write-Warning "Package manager installation failed, trying binary..."
                return Install-FromBinary
            }
            return $true
        }
        "binary" {
            if (-not (Install-FromBinary)) {
                Write-Warning "Binary installation failed, trying source..."
                return Install-FromSource
            }
            return $true
        }
        "source" {
            return Install-FromSource
        }
        default {
            Write-Error "Unknown installation method: $Method"
            return $false
        }
    }
}

# Main execution
function Main {
    Write-Banner
    
    # Enable GUI mode if requested and available
    if ($GUI -and -not (Test-GUIAvailable)) {
        Write-Warning "GUI mode requested but not available, falling back to CLI"
        $GUI = $false
    }
    
    if ($Verbose) {
        Write-Info "Detected platform: $Platform ($Arch)"
        Write-Info "Package manager: $(Get-PackageManager)"
    }
    
    # Handle uninstall
    if ($Uninstall) {
        Uninstall-CppScaffold
        return
    }
    
    # Check if already installed
    $installDir = Get-InstallDirectory
    $executableName = if ($IsWindows) { "cpp-scaffold.exe" } else { "cpp-scaffold" }
    $executable = Join-Path $installDir "bin\$executableName"
    
    if ((Test-Path $executable) -and -not $Force) {
        Write-Warning "cpp-scaffold is already installed at $installDir"
        Write-Info "Use -Force to reinstall or -Uninstall to remove"
        return
    }
    
    # Show GUI confirmation if in GUI mode
    if ($GUI) {
        if (-not (Show-GUIDialog "CPP-Scaffold Installer" "Install cpp-scaffold to $installDir?" "Question")) {
            Write-Info "Installation cancelled by user"
            return
        }
    }
    
    # Perform installation
    Write-Info "Installing cpp-scaffold to: $installDir"
    Write-Info "Installation method: $Method"
    
    if ($DryRun) {
        Write-Info "DRY RUN MODE - No changes will be made"
    }
    
    if (Invoke-MainInstall) {
        if (-not $DryRun) {
            Test-Installation | Out-Null
            Update-Path
        }
        
        Write-Status "Installation completed successfully!"
        
        if ($GUI) {
            Show-GUIDialog "Installation Complete" "cpp-scaffold has been installed successfully!"
        }
        
        Write-Info "Run 'cpp-scaffold --help' to get started"
    } else {
        Write-Error "Installation failed"
        
        if ($GUI) {
            Show-GUIDialog "Installation Failed" "cpp-scaffold installation failed. Check the console for details." "Error"
        }
        
        exit 1
    }
}

# Run main function
Main
