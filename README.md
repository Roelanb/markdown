# Markdown Editor

A feature-rich, cross-platform markdown editor built with C++ and Qt6.

## Features

- **Fast Editing**: Responsive text editor with syntax highlighting
- **Split Editor Mode**: Edit the same document in two synchronized panes side-by-side
- **Live Preview**: Real-time HTML preview of your markdown
- **Synchronized Scrolling**: Editor and preview scroll together
- **File Browser**: Browse and open markdown files from a sidebar
- **Full Markdown Support**:
  - Basic syntax (headings, bold, italic, links, images, lists, etc.)
  - Extended syntax (tables, code blocks, strikethrough, etc.)
  - Syntax highlighting for code blocks in multiple languages
  - Emoji support (GitHub-style emoji codes like `:smile:`)
- **Code Syntax Highlighting**: Support for 20+ programming languages:
  - C, C++, C#, Java, JavaScript, TypeScript
  - Python, Ruby, PHP, Go, Rust
  - Kotlin, Scala, Swift
  - SQL, HTML, XML, CSS, JSON
  - Shell, Makefile, CMake, Gradle
- **Git Integration**: Built-in git operations panel
  - Git status, diff, add, commit, push, pull
  - File selection for staging
  - Real-time command output
  - Keyboard shortcut (Ctrl+G)
- **Menu Bar & Toolbar**: Quick access to common operations
- **Full Screen Mode**: Distraction-free editing experience

## Build Instructions

### Quick Build (Recommended)

The easiest way to build is using the provided build scripts:

**Linux/macOS:**
```bash
./build.sh
```

**Windows:**
```cmd
build.bat
```

Or with PowerShell:
```powershell
.\build.ps1
```

The build script will:
- Check for required dependencies
- Configure the project with CMake
- Build the application
- Show instructions for running

For more options, see [Build Script Options](#build-script-options) below.

### Prerequisites

- CMake 3.16 or higher
- Qt 6.2 or higher (with WebEngine module)
- C++17 compatible compiler

### Manual Build Instructions

### Linux

#### Ubuntu/Debian

1. Install dependencies:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. Run the application:
```bash
./MarkdownEditor
```

4. (Optional) Install system-wide:
```bash
sudo make install
```

#### Fedora/RHEL

1. Install dependencies:
```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. Run the application:
```bash
./MarkdownEditor
```

#### Arch Linux

1. Install dependencies:
```bash
sudo pacman -S cmake qt6-base qt6-webengine
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. Run the application:
```bash
./MarkdownEditor
```

### Windows

#### Using MSVC (Visual Studio)

1. Install Visual Studio 2019 or later with C++ development tools

2. Install Qt 6:
   - Download and install Qt 6 from https://www.qt.io/download
   - Make sure to select the WebEngine component
   - Add Qt to your PATH or note the installation directory

3. Install CMake:
   - Download from https://cmake.org/download/
   - Add CMake to your PATH

4. Build the project:
```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64" ..
cmake --build . --config Release
```

5. Run the application:
```cmd
Release\MarkdownEditor.exe
```

#### Using MinGW

1. Install MinGW-w64

2. Install Qt 6 for MinGW:
   - Download Qt 6 from https://www.qt.io/download
   - Select the MinGW version

3. Build the project:
```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\mingw_64" ..
cmake --build .
```

4. Run the application:
```cmd
MarkdownEditor.exe
```

### macOS

1. Install Xcode Command Line Tools:
```bash
xcode-select --install
```

2. Install Homebrew (if not already installed):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

3. Install dependencies:
```bash
brew install cmake qt@6
```

4. Build the project:
```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
make -j$(sysctl -n hw.ncpu)
```

5. Run the application:
```bash
./MarkdownEditor
```

6. (Optional) Create an application bundle:
```bash
macdeployqt MarkdownEditor.app
```

### Build Script Options

The build scripts support various options for different workflows:

#### Linux/macOS (build.sh)

```bash
./build.sh [OPTION]

Options:
  build          Build the project (default)
  clean          Clean the build directory
  rebuild        Clean and build
  install        Install the application system-wide
  run            Build and run the application
  check          Check dependencies only
  help           Show help message

Examples:
  ./build.sh              # Build the project
  ./build.sh rebuild      # Clean and rebuild
  ./build.sh run          # Build and run
  ./build.sh install      # Install system-wide
```

#### Windows (build.ps1 / build.bat)

```powershell
.\build.ps1 [OPTIONS]

Options:
  -Action <action>     Action to perform (default: build)
                       Actions: build, clean, rebuild, run, check, help
  -QtPath <path>       Path to Qt installation
                       Example: C:\Qt\6.5.0\msvc2019_64
  -Generator <gen>     CMake generator (default: Visual Studio 16 2019)
  -Config <config>     Build configuration (default: Release)

Examples:
  .\build.ps1
  .\build.ps1 -Action rebuild
  .\build.ps1 -Action run -QtPath 'C:\Qt\6.5.0\msvc2019_64'
  .\build.ps1 -Generator 'Visual Studio 17 2022' -Config Debug
```

## Usage

### Opening Files

- **File → Open**: Open a single markdown file
- **File → Open Folder**: Open a folder to browse multiple markdown files
- **File Browser**: Click on files in the left sidebar to open them

### Saving Files

- **File → Save**: Save the current file
- **File → Save As**: Save to a new file

### Editing

- **Edit → Undo/Redo**: Standard undo/redo operations
- **Edit → Cut/Copy/Paste**: Standard clipboard operations
- **Edit → Select All**: Select all text

### View

- **View → Split Editor**: Toggle split editor mode (Ctrl+\ or Cmd+\)
  - Enables side-by-side editing of the same document
  - Both editors are synchronized - changes in one appear in the other
  - Layout: Editor 1 | Editor 2 | Preview
- **View → Git Panel**: Toggle git operations panel (Ctrl+G)
  - Shows changed files in the repository
  - Supports git status, diff, add, commit, push, pull
  - Select files to stage, enter commit message, and commit
  - Real-time command output display
- **View → Full Screen**: Enter full screen mode
- **View → Exit Full Screen**: Exit full screen mode (or press Esc)

### Markdown Syntax

The editor supports all standard markdown syntax:

- **Headings**: `# H1`, `## H2`, etc.
- **Bold**: `**bold**` or `__bold__`
- **Italic**: `*italic*` or `_italic_`
- **Strikethrough**: `~~strikethrough~~`
- **Code**: `` `code` ``
- **Code Blocks**: ``` ```language ```
- **Links**: `[text](url)`
- **Images**: `![alt](url)`
- **Lists**: `- item` or `1. item`
- **Blockquotes**: `> quote`
- **Horizontal Rules**: `---` or `***`
- **Tables**: Standard markdown tables
- **Emojis**: `:smile:`, `:heart:`, etc.

## Troubleshooting

### Linux

**Qt not found:**
```bash
export CMAKE_PREFIX_PATH=/path/to/qt6
```

**WebEngine missing:**
```bash
sudo apt install qt6-webengine-dev  # Ubuntu/Debian
sudo dnf install qt6-qtwebengine-devel  # Fedora
```

### Windows

**Qt not found:**
- Set CMAKE_PREFIX_PATH to your Qt installation directory
- Example: `-DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"`

**Missing DLLs:**
- Copy Qt DLLs to the executable directory or add Qt bin directory to PATH

### macOS

**Qt not found:**
```bash
export CMAKE_PREFIX_PATH=$(brew --prefix qt@6)
```

**Code signing issues:**
```bash
codesign --force --deep --sign - MarkdownEditor.app
```

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
