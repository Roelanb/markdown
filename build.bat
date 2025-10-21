@echo off
REM Markdown Editor Build Script for Windows (Batch wrapper)
REM This script calls the PowerShell build script

setlocal

REM Check if PowerShell is available
where powershell >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: PowerShell is not available
    echo Please install PowerShell or use build.ps1 directly
    exit /b 1
)

REM Get the directory where this script is located
set SCRIPT_DIR=%~dp0

REM Call PowerShell script with all arguments
powershell -ExecutionPolicy Bypass -File "%SCRIPT_DIR%build.ps1" %*

endlocal
