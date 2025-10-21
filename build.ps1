# Markdown Editor Build Script for Windows
# PowerShell script

param(
    [string]$Action = "build",
    [string]$QtPath = "",
    [string]$Generator = "Visual Studio 16 2019",
    [string]$Config = "Release"
)

# Script configuration
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ScriptDir "build"

# Color output functions
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$ForegroundColor = "White"
    )
    Write-Host $Message -ForegroundColor $ForegroundColor
}

function Write-Header {
    param([string]$Message)
    Write-Host ""
    Write-ColorOutput "================================================" -ForegroundColor Cyan
    Write-ColorOutput $Message -ForegroundColor Cyan
    Write-ColorOutput "================================================" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-ColorOutput "✓ $Message" -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$Message)
    Write-ColorOutput "✗ $Message" -ForegroundColor Red
}

function Write-Warning-Custom {
    param([string]$Message)
    Write-ColorOutput "⚠ $Message" -ForegroundColor Yellow
}

function Write-Info {
    param([string]$Message)
    Write-ColorOutput $Message -ForegroundColor Cyan
}

# Check if command exists
function Test-CommandExists {
    param([string]$Command)
    $null -ne (Get-Command $Command -ErrorAction SilentlyContinue)
}

# Detect Qt installation
function Find-QtInstallation {
    Write-Info "Searching for Qt6 installation..."

    # Check common Qt installation paths
    $commonPaths = @(
        "C:\Qt\6.2.0",
        "C:\Qt\6.2.1",
        "C:\Qt\6.2.2",
        "C:\Qt\6.2.3",
        "C:\Qt\6.2.4",
        "C:\Qt\6.3.0",
        "C:\Qt\6.3.1",
        "C:\Qt\6.3.2",
        "C:\Qt\6.4.0",
        "C:\Qt\6.4.1",
        "C:\Qt\6.4.2",
        "C:\Qt\6.4.3",
        "C:\Qt\6.5.0",
        "C:\Qt\6.5.1",
        "C:\Qt\6.5.2",
        "C:\Qt\6.5.3",
        "C:\Qt\6.6.0",
        "C:\Qt\6.6.1"
    )

    $compilerPaths = @("msvc2019_64", "msvc2022_64", "mingw_64")

    foreach ($basePath in $commonPaths) {
        foreach ($compiler in $compilerPaths) {
            $qtPath = Join-Path $basePath $compiler
            if (Test-Path $qtPath) {
                Write-Success "Found Qt6 at: $qtPath"
                return $qtPath
            }
        }
    }

    return $null
}

# Check dependencies
function Test-Dependencies {
    Write-Header "Checking Dependencies"

    $missingDeps = 0

    # Check CMake
    if (Test-CommandExists "cmake") {
        $cmakeVersion = (cmake --version | Select-Object -First 1).Split()[-1]
        Write-Success "CMake found: $cmakeVersion"
    } else {
        Write-Error-Custom "CMake not found"
        $missingDeps++
    }

    # Check for Visual Studio or MinGW
    $hasCompiler = $false
    if (Test-CommandExists "cl") {
        Write-Success "Visual Studio C++ compiler found"
        $hasCompiler = $true
    } elseif (Test-CommandExists "g++") {
        Write-Success "MinGW C++ compiler found"
        $hasCompiler = $true
    }

    if (-not $hasCompiler) {
        Write-Error-Custom "C++ compiler not found"
        Write-Warning-Custom "Please install Visual Studio 2019/2022 with C++ tools or MinGW"
        $missingDeps++
    }

    # Check Qt6
    if ($script:QtPath -eq "") {
        $script:QtPath = Find-QtInstallation
    }

    if ($script:QtPath -eq $null -or $script:QtPath -eq "") {
        Write-Warning-Custom "Qt6 not found automatically"
        Write-Info "Please specify Qt path with -QtPath parameter"
        Write-Info "Example: .\build.ps1 -QtPath 'C:\Qt\6.5.0\msvc2019_64'"
        $missingDeps++
    } else {
        Write-Success "Qt6 will be used from: $script:QtPath"
    }

    if ($missingDeps -gt 0) {
        Write-Host ""
        Write-Error-Custom "Missing required dependencies!"
        Write-Host ""
        Write-Info "Installation instructions:"
        Write-Host "1. CMake: https://cmake.org/download/"
        Write-Host "2. Visual Studio: https://visualstudio.microsoft.com/"
        Write-Host "   (Install 'Desktop development with C++' workload)"
        Write-Host "3. Qt6: https://www.qt.io/download"
        Write-Host "   (Install Qt 6.x with MSVC or MinGW)"
        exit 1
    }

    Write-Host ""
    Write-Success "All required dependencies found"
    return $true
}

# Clean build directory
function Clear-BuildDirectory {
    Write-Header "Cleaning Build Directory"

    if (Test-Path $BuildDir) {
        Write-Warning-Custom "Removing existing build directory..."
        Remove-Item -Recurse -Force $BuildDir
        Write-Success "Build directory cleaned"
    } else {
        Write-Info "No existing build directory found"
    }
}

# Configure with CMake
function Invoke-Configure {
    Write-Header "Configuring Project with CMake"

    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    Push-Location $BuildDir

    $cmakeArgs = @()

    # Add Qt path if specified
    if ($script:QtPath -ne "") {
        $cmakeArgs += "-DCMAKE_PREFIX_PATH=$script:QtPath"
        Write-Info "Using Qt from: $script:QtPath"
    }

    # Add generator
    if ($Generator -ne "") {
        $cmakeArgs += "-G"
        $cmakeArgs += $Generator
        Write-Info "Using generator: $Generator"
    }

    Write-Info "Running CMake..."
    $cmakeCmd = "cmake $($cmakeArgs -join ' ') .."

    try {
        & cmake $cmakeArgs ..
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Configuration successful"
        } else {
            Write-Error-Custom "Configuration failed"
            Pop-Location
            exit 1
        }
    } catch {
        Write-Error-Custom "Configuration failed: $_"
        Pop-Location
        exit 1
    }

    Pop-Location
}

# Build the project
function Invoke-Build {
    Write-Header "Building Project"

    if (-not (Test-Path $BuildDir)) {
        Write-Error-Custom "Build directory not found. Run configure first."
        exit 1
    }

    Push-Location $BuildDir

    Write-Info "Building configuration: $Config"

    try {
        & cmake --build . --config $Config
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Build successful"
        } else {
            Write-Error-Custom "Build failed"
            Pop-Location
            exit 1
        }
    } catch {
        Write-Error-Custom "Build failed: $_"
        Pop-Location
        exit 1
    }

    Pop-Location
}

# Run the application
function Start-Application {
    Write-Header "Running Application"

    $exePath = Join-Path $BuildDir "$Config\MarkdownEditor.exe"

    if (Test-Path $exePath) {
        Write-Success "Starting Markdown Editor..."
        & $exePath
    } else {
        Write-Error-Custom "Executable not found at: $exePath"
        Write-Warning-Custom "Build may have failed or configuration is incorrect"
        exit 1
    }
}

# Print usage information
function Show-Usage {
    Write-Host "Markdown Editor Build Script for Windows"
    Write-Host ""
    Write-Host "Usage: .\build.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Action <action>     Action to perform (default: build)"
    Write-Host "                       Actions: build, clean, rebuild, run, check, help"
    Write-Host "  -QtPath <path>       Path to Qt installation"
    Write-Host "                       Example: C:\Qt\6.5.0\msvc2019_64"
    Write-Host "  -Generator <gen>     CMake generator (default: Visual Studio 16 2019)"
    Write-Host "                       Options: 'Visual Studio 16 2019'"
    Write-Host "                                'Visual Studio 17 2022'"
    Write-Host "                                'MinGW Makefiles'"
    Write-Host "  -Config <config>     Build configuration (default: Release)"
    Write-Host "                       Options: Debug, Release"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1"
    Write-Host "  .\build.ps1 -Action rebuild"
    Write-Host "  .\build.ps1 -Action run -QtPath 'C:\Qt\6.5.0\msvc2019_64'"
    Write-Host "  .\build.ps1 -Generator 'Visual Studio 17 2022' -Config Debug"
}

# Main script logic
function Main {
    switch ($Action.ToLower()) {
        "build" {
            Test-Dependencies
            Invoke-Configure
            Invoke-Build
            Write-Header "Build Complete!"
            Write-Success "Build successful"
            Write-Host ""
            Write-Info "To run the application:"
            Write-Host "  cd build\$Config"
            Write-Host "  .\MarkdownEditor.exe"
            Write-Host ""
            Write-Info "Or use:"
            Write-Host "  .\build.ps1 -Action run"
        }

        "clean" {
            Clear-BuildDirectory
            Write-Success "Clean complete"
        }

        "rebuild" {
            Test-Dependencies
            Clear-BuildDirectory
            Invoke-Configure
            Invoke-Build
            Write-Header "Rebuild Complete!"
            Write-Success "Rebuild successful"
        }

        "run" {
            $exePath = Join-Path $BuildDir "$Config\MarkdownEditor.exe"
            if (-not (Test-Path $exePath)) {
                Write-Warning-Custom "Executable not found. Building first..."
                Test-Dependencies
                Invoke-Configure
                Invoke-Build
            }
            Start-Application
        }

        "check" {
            Test-Dependencies
        }

        "help" {
            Show-Usage
        }

        default {
            Write-Error-Custom "Unknown action: $Action"
            Write-Host ""
            Show-Usage
            exit 1
        }
    }
}

# Execute main function
Main
