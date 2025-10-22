# Image Drag-and-Drop Feature

## Overview

The markdown editor now supports drag-and-drop insertion of images from the file browser directly into the editor. This feature automatically generates the correct markdown image syntax with relative paths.

## How It Works

### User Experience

1. **Open a markdown file** (or create a new one and save it)
2. **Open a folder** containing images via File → Open Folder
3. **Drag an image file** from the file browser on the left
4. **Drop it into the editor** at the desired location
5. **Markdown image syntax is automatically inserted** at the exact drop location

### Example

If you drag `screenshot.png` from the file browser, the editor will insert:

```markdown
![screenshot](relative/path/to/screenshot.png)
```

The path is automatically calculated relative to the current markdown file's location.

## Supported Image Formats

- PNG (`.png`)
- JPEG (`.jpg`, `.jpeg`)
- GIF (`.gif`)
- BMP (`.bmp`)
- SVG (`.svg`)
- WebP (`.webp`)
- ICO (`.ico`)

## Path Calculation

### Relative Paths (Saved Files)

When the markdown file is saved, the editor calculates the relative path from the markdown file to the image:

**Example:**
- Markdown file: `/home/user/docs/article.md`
- Image file: `/home/user/docs/images/photo.png`
- Inserted: `![photo](images/photo.png)`

**Another Example:**
- Markdown file: `/home/user/docs/blog/post.md`
- Image file: `/home/user/docs/images/banner.jpg`
- Inserted: `![banner](../images/banner.jpg)`

### Absolute Paths (Unsaved Files)

If the markdown file hasn't been saved yet, the editor uses the absolute path:

**Example:**
- Image file: `/home/user/pictures/diagram.png`
- Inserted: `![diagram](/home/user/pictures/diagram.png)`

## Implementation Details

### Modified Files

1. **`src/markdowneditor.h`**
   - Added `setCurrentFilePath()` and `getCurrentFilePath()` methods
   - Added drag-and-drop event handlers
   - Added helper methods for path calculation and image insertion

2. **`src/markdowneditor.cpp`**
   - Implemented `dragEnterEvent()` to accept image files
   - Implemented `dragMoveEvent()` to track drag movement
   - Implemented `dropEvent()` to handle the drop and insert markdown
   - Implemented `insertImageMarkdown()` to generate the markdown syntax
   - Implemented `calculateRelativePath()` to compute relative paths

3. **`src/filebrowser.cpp`**
   - Enabled drag-and-drop in the tree view
   - Added image file extensions to the file filter

4. **`src/mainwindow.cpp`**
   - Updated `loadFile()` to set the current file path in the editor
   - Updated `saveFileToPath()` to set the current file path after saving
   - Updated `newFile()` to clear the file path

### Technical Approach

**Drag Detection:**
- The editor checks if the dragged data contains file URLs
- It validates that at least one file has an image extension
- Only image files are accepted for dropping

**Path Calculation:**
- Uses Qt's `QDir::relativeFilePath()` to compute relative paths
- Gets the directory of the markdown file
- Calculates the relative path from that directory to the image

**Markdown Generation:**
- Alt text is set to the image filename (without extension)
- Format: `![alt-text](path/to/image.ext)`
- Inserted at the exact drop position using `cursorForPosition()`

## Usage Tips

### Best Practices

1. **Save your markdown file first** to get relative paths
2. **Keep images in the same folder structure** as your markdown files
3. **Use descriptive filenames** - they become the alt text
4. **Organize images in subfolders** (e.g., `images/`, `assets/`)

### Workflow Example

```
project/
├── docs/
│   ├── README.md          ← Your markdown file
│   └── images/
│       ├── logo.png
│       └── screenshot.jpg
```

When you drag `logo.png` into `README.md`, it inserts:
```markdown
![logo](images/logo.png)
```

### Multiple Images

You can drag and drop multiple images one at a time. Each drop inserts a new image at the cursor position.

## Preview

The live preview automatically renders the inserted images, so you can immediately see how they look in your document.

## Keyboard Alternative

While drag-and-drop is the primary method, you can also manually type the markdown image syntax:

```markdown
![Alt text](path/to/image.png)
```

## Future Enhancements

Potential improvements:
- Support for dragging multiple images at once
- Image resizing in markdown (using HTML)
- Copy-paste images from clipboard
- Image optimization/compression
- Drag from external file managers (not just the file browser)
- Custom alt text prompt on drop

## Troubleshooting

**Problem:** Absolute paths are inserted instead of relative paths
- **Solution:** Make sure to save your markdown file first

**Problem:** Image doesn't appear in preview
- **Solution:** Check that the path is correct and the image file exists

**Problem:** Can't drag images from file browser
- **Solution:** Make sure you've opened a folder (File → Open Folder) and the file browser shows image files

## Conclusion

The drag-and-drop image insertion feature makes it quick and easy to add images to your markdown documents with proper relative paths, improving the portability and maintainability of your documentation.
