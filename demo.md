# Markdown Editor Demo

Welcome to the **Markdown Editor**! This file demonstrates all the supported features.

## Basic Syntax

hello

### Text Formatting

- **Bold text** using `**bold**`
- *Italic text* using `*italic*`
- ***Bold and italic*** using `***both***`
- ~~Strikethrough~~ using `~~strikethrough~~`

### Lists

#### Unordered Lists
- Item 1
- Item 2
  - Nested item 2.1
  - Nested item 2.2
- Item 3

#### Ordered Lists
1. First item
2. Second item
3. Third item
   1. Nested 3.1
   2. Nested 3.2

#### Task Lists
- [x] Completed task
- [ ] Incomplete task
- [ ] Another task

### Links and Images

[Visit GitHub](https://github.com)

![Markdown Logo](https://markdown-here.com/img/icon256.png)

### Blockquotes

> This is a blockquote.
>
> It can span multiple lines.
>> And can be nested!

### Code

Inline code: `const greeting = "Hello, World!";`

Code blocks with syntax highlighting:

```javascript
function fibonacci(n) {
  if (n <= 1) return n;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

console.log(fibonacci(10));
```

```python
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print(factorial(5))
```

```go
package main

import "fmt"

func main() {
    fmt.Println("Hello, Go!")
}
```

### Horizontal Rules

---

## Extended Syntax

### Tables

| Feature | Supported | Notes |
|---------|-----------|-------|
| Basic Markdown | ✓ | All standard features |
| Tables | ✓ | GitHub Flavored Markdown |
| Emoji | ✓ | Use `:shortcode:` syntax |
| Syntax Highlighting | ✓ | 20+ languages |

### Footnotes

Here's a sentence with a footnote[^1].

[^1]: This is the footnote content.

### Definition Lists

Term 1
: Definition 1

Term 2
: Definition 2a
: Definition 2b

### Emojis

You can use emoji shortcodes:

- :smile: Happy
- :heart: Love
- :thumbsup: Approve
- :rocket: Launch
- :fire: Hot
- :star: Favorite
- :tada: Celebration
- :coffee: Coffee
- :computer: Coding

### Advanced Code Blocks

```json
{
  "name": "markdown-editor",
  "version": "1.0.0",
  "features": ["fast", "cross-platform", "full-featured"]
}
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<note>
  <to>User</to>
  <from>Markdown Editor</from>
  <heading>Demo</heading>
  <body>Enjoy the editor!</body>
</note>
```

```sql
SELECT users.name, COUNT(posts.id) as post_count
FROM users
LEFT JOIN posts ON users.id = posts.user_id
GROUP BY users.id
ORDER BY post_count DESC
LIMIT 10;
```

---

## Math and Special Characters

Using typographer:

- Smart quotes: "Hello"
- Em dashes: This is --- an em dash
- Ellipsis: Wait...

---

## Conclusion

This markdown editor supports:
- ✓ All basic markdown syntax
- ✓ GitHub Flavored Markdown (GFM)
- ✓ Extended syntax (tables, task lists, footnotes)
- ✓ Syntax highlighting for 20+ languages
- ✓ Emoji support
- ✓ Live preview
- ✓ Synchronized scrolling
- ✓ Cross-platform (Linux, Windows, macOS)

**Happy writing!** :pencil:
