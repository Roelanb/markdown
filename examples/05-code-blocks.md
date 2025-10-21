# Code Blocks

This file demonstrates inline code and code blocks with syntax highlighting.

## Inline Code

Use single backticks for `inline code` within text.

Examples:
- The `console.log()` function prints to the console
- Use `git commit -m "message"` to commit changes
- The variable `userName` stores the name

## Code Blocks

Use triple backticks for multi-line code blocks.

### Python

```python
def hello():
    print("Hello, World!")

# Call the function
hello()
```

### JavaScript

```javascript
function greet(name) {
    console.log(`Hello, ${name}!`);
}

greet("World");
```

### C++

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

### Java

```java
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello, World!");
    }
}
```

### HTML

```html
<!DOCTYPE html>
<html>
<head>
    <title>My Page</title>
</head>
<body>
    <h1>Hello, World!</h1>
</body>
</html>
```

### CSS

```css
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 20px;
}

h1 {
    color: #333;
    font-size: 2em;
}
```

### JSON

```json
{
    "name": "John Doe",
    "age": 30,
    "city": "New York",
    "skills": ["JavaScript", "Python", "C++"]
}
```

### Shell/Bash

```bash
#!/bin/bash

echo "Hello, World!"
cd /home/user
ls -la
```

### SQL

```sql
SELECT name, age, city
FROM users
WHERE age > 25
ORDER BY name;
```

### Plain Text

```
This is a plain text code block
without syntax highlighting.
It's useful for generic content.
```
