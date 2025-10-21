# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Markdown editor written in C with two implementations:
1. **Linux/Terminal version** - Uses ncurses for terminal UI
2. **Windows version** - Uses Win32 API for native GUI

Key features:
- Fast editing with gap buffer data structure
- Syntax highlighting
- Live preview in split-pane view

## Build System

### Linux/Terminal Version
```bash
make              # Build ncurses version
make clean        # Remove build artifacts
./mdedit [file]   # Run the editor
```

**Dependencies**: ncurses library
```bash
# Ubuntu/Debian
sudo apt-get install libncurses5-dev libncursesw5-dev
```

### Windows Version

**Native Windows build:**
```cmd
make -f Makefile.win     # Requires MinGW on Windows
```

**Cross-compile from Linux:**
```bash
sudo apt-get install mingw-w64
make -f Makefile.mingw   # Creates mdedit.exe
```

## Architecture

### Core Components

**buffer.c/h** - Gap buffer implementation
- Efficient text insertion/deletion at cursor position
- O(1) operations for cursor-local edits
- Dynamic memory growth
- File I/O operations

**Linux Version:**
- **main.c** - Entry point with ncurses initialization
- **editor.c/h** - Terminal UI and keyboard handling
- **highlight.c/h** - Markdown syntax highlighting using ncurses colors
- **preview.c/h** - Terminal-based markdown preview rendering

**Windows Version:**
- **win32_main.c** - WinMain entry point
- **win32_editor.c/h** - Win32 window management and UI
- Uses native Windows Edit and RichEdit controls
- Standard Windows menus and dialogs

### Key Design Patterns

**Gap Buffer**: Text editing uses gap buffer for O(1) insertions/deletions at cursor position. See buffer.c:40-60 for implementation.

**Split Pane Rendering**:
- Linux: Uses ncurses windows with manual line-by-line rendering
- Windows: Uses WM_SIZE message to resize child controls proportionally

**Live Preview**: Updates on every text change
- Linux: Re-renders on each keystroke in editor_render()
- Windows: EN_CHANGE notification triggers UpdatePreview()

## Important Implementation Notes

**String Parsing**: Avoid `strtok()` - it's stateful and causes bugs when parsing multiple strings. Use manual parsing with loops (see editor.c:77-87).

**Memory Management**: Always check `strdup()`, `malloc()` return values. Free allocated strings before overwriting pointers.

**Cross-platform**: buffer.c is shared between both versions. Keep it platform-independent.
