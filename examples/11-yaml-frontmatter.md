---
title: YAML Front Matter Example
author: John Doe
date: 2025-10-21
tags: [markdown, yaml, documentation]
description: This document demonstrates YAML front matter in Markdown files
version: 1.0
status: published
---

# YAML Front Matter

This document demonstrates the use of YAML front matter in Markdown files.

## What is YAML Front Matter?

YAML front matter is a block of metadata at the beginning of a Markdown file, enclosed between triple-dashed lines (`---`). It's commonly used in static site generators like Jekyll, Hugo, and others.

## Structure

The front matter must:
- Start at the very beginning of the file
- Begin with `---` on the first line
- End with `---` or `...` on a line by itself
- Contain valid YAML syntax

## Common Use Cases

### Blog Posts

```yaml
---
title: My Blog Post
author: Jane Smith
date: 2025-10-21
categories: [technology, programming]
tags: [javascript, web-development]
---
```

### Documentation

```yaml
---
title: API Reference
version: 2.0
last_updated: 2025-10-21
status: stable
---
```

### Static Site Pages

```yaml
---
layout: default
permalink: /about/
title: About Us
---
```

## Supported Data Types

YAML front matter supports various data types:

- **Strings**: `title: My Document`
- **Numbers**: `version: 1.0`
- **Booleans**: `published: true`
- **Arrays**: `tags: [tag1, tag2, tag3]`
- **Objects**: 
  ```yaml
  author:
    name: John Doe
    email: john@example.com
  ```

## Benefits

1. **Metadata Storage** - Store document metadata separately from content
2. **Template Processing** - Static site generators can use this data
3. **Organization** - Categorize and tag content easily
4. **Automation** - Enable automated processing and sorting

## Example with Complex Metadata

The front matter at the top of this file contains:
- Document title
- Author information
- Publication date
- Tags for categorization
- Description
- Version number
- Publication status

This metadata can be used by various tools to process, organize, and display the document appropriately.
