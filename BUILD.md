# Build Documentation

Comprehensive guide for building the Markdown Editor on all supported platforms.

## Table of Contents

- [Quick Build (Recommended)](#quick-build-recommended)
- [Build Scripts](#build-scripts)
- [Manual Build](#manual-build)
- [Troubleshooting](#troubleshooting)
- [Advanced Configuration](#advanced-configuration)

## Quick Build (Recommended)

The easiest way to build the Markdown Editor is using the provided build scripts.

### Linux/macOS

```bash
./build.sh
```

### Windows

Using Command Prompt:
```cmd
build.bat
```

Using PowerShell:
```powershell
.\build.ps1
```

The build scripts will automatically:
1. Check for required dependencies
2. Configure the project with CMake
3. Build the application
4. Show instructions for running

## Build Scripts

### Linux/macOS (build.sh)

#### Usage

```bash
./build.sh [OPTION]
```

#### Options

| Option    | Description                              |
|-----------|------------------------------------------|
| `build`   | Build the project (default)              |
| `clean`   | Clean the build directory                |
| `rebuild` | Clean and rebuild from scratch           |
| `install` | Install the application system-wide      |
| `run`     | Build and run the application            |
| `check`   | Check dependencies only                  |
| `help`    | Show help message                        |

#### Examples

```bash
# Build the project
./build.sh

# Clean and rebuild
./build.sh rebuild

# Build and run immediately
./build.sh run

# Install system-wide (requires sudo)
./build.sh install

# Check if dependencies are installed
./build.sh check
```

#### Features

- Automatic dependency detection
- Color-coded output for easy reading
- Platform-specific instructions (Linux vs macOS)
- Automatic Qt6 detection on macOS (Homebrew)
- Multi-core parallel builds
- Error handling with helpful messages

### Windows (build.ps1 / build.bat)

#### Usage

PowerShell:
```powershell
.\build.ps1 [OPTIONS]
```

Command Prompt:
```cmd
build.bat [OPTIONS]
```

#### Options

| Option        | Description                                   | Default                 |
|---------------|-----------------------------------------------|-------------------------|
| `-Action`     | Action to perform                             | `build`                 |
| `-QtPath`     | Path to Qt installation                       | Auto-detect             |
| `-Generator`  | CMake generator to use                        | `Visual Studio 16 2019` |
| `-Config`     | Build configuration (Debug/Release)           | `Release`               |

#### Actions

- `build` - Build the project
- `clean` - Clean the build directory
- `rebuild` - Clean and rebuild from scratch
- `run` - Build and run the application
- `check` - Check dependencies only
- `help` - Show help message

#### Examples

```powershell
# Build the project
.\build.ps1

# Build with custom Qt path
.\build.ps1 -QtPath "C:\Qt\6.5.0\msvc2019_64"

# Build with Visual Studio 2022
.\build.ps1 -Generator "Visual Studio 17 2022"

# Build debug version
.\build.ps1 -Config Debug

# Clean and rebuild
.\build.ps1 -Action rebuild

# Build and run
.\build.ps1 -Action run

# Check dependencies
.\build.ps1 -Action check
```

#### Features

- Automatic Qt6 detection (searches common paths)
- Support for Visual Studio 2019/2022 and MinGW
- Color-coded output
- Comprehensive error messages
- Automatic dependency checking

## Manual Build

If you prefer to build manually or the build scripts don't work for your setup:

### Prerequisites

All Platforms:
- CMake 3.16 or higher
- Qt 6.2 or higher (with WebEngine module)
- C++17 compatible compiler

### Linux

#### Ubuntu/Debian

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build (using all CPU cores)
make -j$(nproc)

# Run
./MarkdownEditor

# (Optional) Install system-wide
sudo make install
```

#### Fedora/RHEL

```bash
# Install dependencies
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
./MarkdownEditor
```

#### Arch Linux

```bash
# Install dependencies
sudo pacman -S cmake qt6-base qt6-webengine

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
./MarkdownEditor
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake qt@6

# Build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
make -j$(sysctl -n hw.ncpu)

# Run
./MarkdownEditor

# (Optional) Create app bundle
macdeployqt MarkdownEditor.app
```

### Windows

#### Visual Studio (MSVC)

```cmd
REM Install Visual Studio 2019/2022 with C++ development tools
REM Install Qt 6 from https://www.qt.io/download
REM Install CMake from https://cmake.org/download

REM Create build directory
mkdir build
cd build

REM Configure (adjust Qt path as needed)
cmake -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64" ..

REM Build
cmake --build . --config Release

REM Run
Release\MarkdownEditor.exe
```

#### MinGW

```cmd
REM Install MinGW-w64
REM Install Qt 6 for MinGW from https://www.qt.io/download

REM Create build directory
mkdir build
cd build

REM Configure
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\mingw_64" ..

REM Build
cmake --build .

REM Run
MarkdownEditor.exe
```

## Troubleshooting

### Common Issues

#### Qt6 not found

**Linux:**
```bash
# Make sure Qt6 is installed
sudo apt install qt6-base-dev qt6-webengine-dev  # Ubuntu/Debian
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel  # Fedora

# If installed in non-standard location, set CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH=/path/to/qt6
```

**macOS:**
```bash
# Ensure Qt6 is installed via Homebrew
brew install qt@6

# Set Qt path explicitly
export CMAKE_PREFIX_PATH=$(brew --prefix qt@6)
```

**Windows:**
```powershell
# Specify Qt path in build script
.\build.ps1 -QtPath "C:\Qt\6.5.0\msvc2019_64"

# Or set environment variable
$env:CMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"
```

#### WebEngine module missing

```bash
# Linux (Ubuntu/Debian)
sudo apt install qt6-webengine-dev

# Linux (Fedora)
sudo dnf install qt6-qtwebengine-devel

# Linux (Arch)
sudo pacman -S qt6-webengine

# macOS (included with qt@6)
brew reinstall qt@6

# Windows: Ensure WebEngine is selected during Qt installation
```

#### CMake version too old

```bash
# Linux: Install from Kitware's repository
# https://apt.kitware.com/

# macOS
brew upgrade cmake

# Windows: Download latest from cmake.org
```

#### Compiler not found

**Linux:**
```bash
sudo apt install build-essential  # Ubuntu/Debian
sudo dnf install gcc-c++  # Fedora
```

**macOS:**
```bash
xcode-select --install
```

**Windows:**
- Install Visual Studio with "Desktop development with C++" workload
- Or install MinGW-w64

#### Build fails with "permission denied"

```bash
# Make build script executable (Linux/macOS)
chmod +x build.sh

# Run with sudo for system-wide installation
sudo ./build.sh install
```

## Advanced Configuration

### Custom CMake Options

```bash
# Specify build type
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Specify installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/opt/markdown-editor ..

# Enable verbose build output
cmake --build . --verbose

# Use specific number of build threads
cmake --build . -j4
```

### Cross-Compilation

The build system supports cross-compilation. Set the appropriate CMake toolchain file:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake ..
```

### Static Build

To create a static build (single executable with no external dependencies):

1. Build Qt6 statically (complex process, see Qt documentation)
2. Configure with static Qt:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/path/to/static/qt ..
   ```

### Custom Qt Location

If Qt is installed in a non-standard location:

```bash
# Linux/macOS
cmake -DCMAKE_PREFIX_PATH=/custom/path/to/qt6 ..

# Windows
cmake -DCMAKE_PREFIX_PATH="C:\custom\qt\path" ..
```

## Development Builds

For development with faster rebuild times:

```bash
# Use Debug build
./build.sh
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Enable compiler warnings
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra" ..

# Use ccache for faster rebuilds (if installed)
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ..
```

## CI/CD Integration

The build scripts are designed to work in CI/CD environments:

```bash
# Non-interactive build
./build.sh build

# Check exit code
if [ $? -eq 0 ]; then
    echo "Build successful"
else
    echo "Build failed"
    exit 1
fi
```

## Getting Help

If you encounter issues:

1. Check the [troubleshooting section](#troubleshooting)
2. Run dependency check: `./build.sh check` (Linux/macOS) or `.\build.ps1 -Action check` (Windows)
3. Review the full build output for error messages
4. Check that all prerequisites are installed
5. Consult the main README.md for additional information

For bug reports, please include:
- Operating system and version
- Qt version
- CMake version
- Compiler version
- Full build output
