# Quick Start Guide

## Easy Build (Recommended)

### Linux/macOS

```bash
# Run the build script
./build.sh

# Or build and run immediately
./build.sh run
```

### Windows

```cmd
# Run the build script
build.bat

# Or with PowerShell
.\build.ps1 -Action run
```

The build script automatically:
- Checks for dependencies
- Configures the project
- Builds the application
- Shows you how to run it

---

## Manual Build

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./MarkdownEditor
```

### macOS

```bash
# Install dependencies
brew install cmake qt@6

# Build
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
make -j$(sysctl -n hw.ncpu)

# Run
./MarkdownEditor
```

### Windows

```cmd
# Install Qt 6 from https://www.qt.io/download
# Install CMake from https://cmake.org/download

# Build with Visual Studio
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64" ..
cmake --build . --config Release

# Run
Release\MarkdownEditor.exe
```

## First Steps

1. **Open a File**: Click "File → Open" or use the file browser on the left
2. **Open a Folder**: Click "File → Open Folder" to browse multiple markdown files
3. **Start Editing**: Type markdown in the editor pane on the left
4. **See Live Preview**: The preview pane on the right updates in real-time
5. **Try Emoji**: Type `:smile:` and see it render as 😄
6. **Code Blocks**: Use triple backticks with language name for syntax highlighting

## Example Markdown

```markdown
# Welcome to Markdown Editor

This is a **bold** statement and this is *italic*.

## Features

- Fast editing
- Live preview :rocket:
- Syntax highlighting

### Code Example

` ``python
def hello():
    print("Hello, World!")
` ``

### Table

| Feature | Status |
|---------|--------|
| Editor  | ✅     |
| Preview | ✅     |

> This is a blockquote

Check out the [documentation](https://www.markdownguide.org/)!
```
