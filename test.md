# Markdown Spec Compliance Test

## HTML Escaping Test
This should display as text: <script>alert('xss')</script>
Math: 5 < 10 && 10 > 5

## Code Span Test
This should be bold with code: **bold `code** should` continue

Inline code should be literal: `**not bold**` and `<html>tags</html>`

## Bold/Italic Nesting
***This should be bold AND italic***
**This is bold with *italic* inside**
*This is italic with **bold** inside*

## Header Without Space
#NoSpaceHeader
######SixHashesNoSpace

## Blockquote Tests
> Single line quote

> Multi-line quote
> continues here
> and here

> Level 1
> > Level 2 nested

## List Tests
- Item 1
  - Nested item 1.1
  - Nested item 1.2
- Item 2

1. First
2. Second
   1. Nested 2.1
   2. Nested 2.2

- [ ] Unchecked task
- [x] Checked task

## Link/Image Tests
[Link with [brackets] inside](https://example.com)
\[Not a link\]

![Image](test.jpg)

## Code Block Tests
```js code on same line```

    Indented code block
    with multiple lines

## Horizontal Rule Tests
---
***
___
-*- (should NOT be a rule)

## Hard Line Break
Two spaces at end
should create line break

## Autolinks
<https://example.com>
<user@example.com>

## Escape Tests
\*Not italic\*
\[Not a link\]

## Reference Links
[Reference link][ref]
[Another reference]

[ref]: https://example.com "Title"
[Another reference]: https://example.org
