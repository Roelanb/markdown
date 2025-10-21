# Markdown Editor - GTK Version

A fast markdown editor written in C with GTK3 and WebKit2GTK for live preview.

## Features

- **Fast editing** - Gap buffer data structure for O(1) insertions/deletions at cursor
- **Syntax highlighting** - Real-time markdown syntax highlighting in the editor
- **Live preview** - Split-pane view with rendered HTML preview using WebKit2GTK
- **Code syntax highlighting** - Automatic syntax highlighting for 190+ programming languages in code blocks
- **Emoji support** - GitHub-style emoji shortcodes with 500+ emojis (`:smile:`, `:heart:`, `:rocket:`, etc.)

## Dependencies

### Ubuntu/Debian
```bash
sudo apt-get install libgtk-3-dev libwebkit2gtk-4.1-dev
```

### Fedora/RHEL
```bash
sudo dnf install gtk3-devel webkit2gtk3-devel
```

### Arch Linux
```bash
sudo pacman -S gtk3 webkit2gtk
```

## Building

```bash
make -f Makefile.gtk
```

## Running

```bash
./mdedit-gtk [file.md]
```

If no file is specified, the editor opens with a welcome message demonstrating the markdown features.

## Usage

### File Operations
- **Open**: File menu → Open, or click the "Open" toolbar button
- **Save**: File menu → Save, or click the "Save" toolbar button
- **Save As**: File menu → Save as, or click the "Save As" toolbar button

### Editing
- **Undo**: Edit menu → Undo, or click the "Undo" toolbar button
- **Redo**: Edit menu → Redo, or click the "Redo" toolbar button
- **Cut**: Edit menu → Cut, or click the "Cut" toolbar button
- **Copy**: Edit menu → Copy, or click the "Copy" toolbar button
- **Paste**: Edit menu → Paste, or click the "Paste" toolbar button
- **Select All**: Edit menu → Select all, or click the "Select All" toolbar button

### Viewing
- **Full Screen**: View menu → Full screen, or click the "Fullscreen" toolbar button
- **Exit Full Screen**: View menu → Exit full screen, or click the "Exit Fullscreen" toolbar button
- **Live Preview**: Type in the left pane to see live preview in the right pane
- **Synchronized Scrolling**: The preview pane automatically scrolls with the editor
- **Drag**: Resize the split pane by dragging the divider

## Supported Markdown Features

Complete support for all [Markdown Basic Syntax](https://www.markdownguide.org/basic-syntax/) and [Extended Syntax](https://www.markdownguide.org/extended-syntax/):

### Headings
- All 6 levels: `# H1` through `###### H6`
- Larger styling for H1-H2, distinct styling for H3-H6

### Emphasis
- **Bold**: `**text**` or `__text__`
- *Italic*: `*text*` or `_text_`
- ***Bold and Italic***: `***text***`

### Lists
- **Unordered lists**: `-`, `*`, or `+` followed by space
- **Ordered lists**: `1.` followed by space
- Automatic numbering in preview

### Code
- **Inline code**: `` `code` ``
- **Code blocks**: Triple backticks (```) with optional language specification
  ```python
  def example():
      return "Hello"
  ```

### Blockquotes
- Use `>` at the start of lines
- Supports multiple paragraphs
- Styled with left border and italic text

### Links and Images
- **Links**: `[link text](url)`
- **Images**: `![alt text](image-url)`
- Images are responsive and scale to fit

### Horizontal Rules
- Create with `---`, `***`, or `___` (3 or more)
- Rendered as subtle divider line

### Paragraphs
- Separate paragraphs with blank lines
- Single line breaks become spaces
- Proper paragraph spacing in preview

## Extended Markdown Features

### Emoji

- GitHub-style emoji shortcodes: `:shortcode:`
- Over 500 emojis supported
- Common examples: `:smile:` 😄, `:heart:` ❤️, `:thumbsup:` 👍, `:rocket:` 🚀
- Categories: Smileys, People, Animals, Food, Activities, Travel, Objects, Symbols
- See full documentation below for complete emoji list

### Tables
- Pipe-based table syntax: `| Header | Header |`
- Header separator row: `|--------|--------|`
- Automatic styling with borders and alternating row colors
- Support for multiple columns and rows

Example:
```markdown
| Feature | Status |
|---------|--------|
| Tables  | ✓      |
| Lists   | ✓      |
```

### Task Lists
- Checkbox syntax: `- [ ]` for unchecked, `- [x]` for checked
- Interactive checkboxes in preview (display only)
- Perfect for TODO lists and project tracking

Example:
```markdown
- [x] Completed task
- [ ] Pending task
```

### Strikethrough
- Use double tildes: `~~text~~`
- Renders with strikethrough styling
- Great for showing deletions or deprecated content

### Highlight
- Use double equals: `==text==`
- Renders with yellow background highlight
- Perfect for emphasizing important content

### Fenced Code Blocks with Syntax Highlighting
- Triple backticks with optional language specification
- **Full syntax highlighting** powered by highlight.js
- Support for 190+ languages including:
  - **Web**: HTML, CSS, JavaScript, TypeScript, JSON, XML
  - **Systems**: C, C++, C#, Rust, Go
  - **Scripting**: Python, Ruby, PHP, Shell, Bash
  - **JVM**: Java, Kotlin, Scala
  - **Mobile**: Swift
  - **Database**: SQL
  - **Build**: Makefile, CMake, Gradle
  - And many more!

Example:
````markdown
```python
def hello_world():
    print("Hello!")
```

```javascript
const greet = () => console.log("Hi!");
```

```c
#include <stdio.h>
int main() { return 0; }
```
````

The syntax highlighting automatically detects the language and applies appropriate color coding for keywords, strings, comments, functions, and more.

### Emoji Support

- Use GitHub-style emoji shortcodes: `:shortcode:`
- **500+ emojis supported** including all common GitHub emojis
- Automatic conversion to Unicode emoji characters in preview

Example shortcodes:
```markdown
:smile: :heart: :thumbsup: :tada: :rocket: :fire:
:wave: :sunglasses: :100: :sparkles:
:joy: :heart_eyes: :thinking: :cry:
:coffee: :pizza: :computer: :books:
:dog: :cat: :panda_face: :unicorn:
:sunny: :rainbow: :star: :snowflake:
```

Popular categories:
- **Smileys & Emotions**: `:smile:` `:heart:` `:joy:` `:thumbsup:` `:fire:`
- **People & Body**: `:wave:` `:clap:` `:muscle:` `:pray:` `:facepalm:`
- **Animals & Nature**: `:dog:` `:cat:` `:tree:` `:flower:` `:rainbow:`
- **Food & Drink**: `:pizza:` `:coffee:` `:cake:` `:apple:` `:beer:`
- **Activities**: `:soccer:` `:basketball:` `:guitar:` `:art:` `:trophy:`
- **Travel & Places**: `:car:` `:airplane:` `:rocket:` `:house:` `:beach:`
- **Objects**: `:computer:` `:phone:` `:books:` `:gift:` `:bulb:`
- **Symbols**: `:100:` `:heart:` `:star:` `:checkmark:` `:warning:`

For a complete list of supported emojis, see the [GitHub Emoji Cheat Sheet](https://github.com/ikatyang/emoji-cheat-sheet).

## Architecture

- **buffer.c/h** - Gap buffer for efficient text editing
- **gtk_editor.c/h** - GTK UI and editor management
- **highlight.c/h** - Syntax highlighting and markdown-to-HTML conversion
- **emoji.c/h** - Emoji shortcode to Unicode mapping (500+ emojis)
- **gtk_main.c** - Application entry point

The gap buffer provides O(1) complexity for insertions and deletions at the cursor position, making editing very fast even for large documents.
