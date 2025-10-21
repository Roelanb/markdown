#include <gtk/gtk.h>
#include "gtk_editor.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    Editor *editor = editor_new();
    if (!editor) {
        g_printerr("Failed to create editor\n");
        return 1;
    }

    /* Open file if provided as argument */
    if (argc > 1) {
        editor_open_file(editor, argv[1]);
    } else {
        /* Set initial welcome text */
        const char *welcome = "# Welcome to Markdown Editor\n\n"
                             "Start typing to see **live preview** with synchronized scrolling!\n\n"
                             "## All Markdown Basic Syntax Supported\n\n"
                             "### Headings\n"
                             "# H1 Heading\n"
                             "## H2 Heading\n"
                             "### H3 Heading\n"
                             "#### H4 Heading\n"
                             "##### H5 Heading\n"
                             "###### H6 Heading\n\n"
                             "### Emphasis\n"
                             "**Bold text** or __also bold__\n\n"
                             "*Italic text* or _also italic_\n\n"
                             "***Bold and italic*** combined!\n\n"
                             "### Lists\n"
                             "Unordered list:\n"
                             "- Item 1\n"
                             "- Item 2\n"
                             "- Item 3\n\n"
                             "Ordered list:\n"
                             "1. First item\n"
                             "2. Second item\n"
                             "3. Third item\n\n"
                             "### Code with Syntax Highlighting\n"
                             "Inline `code snippets` look like this.\n\n"
                             "Python:\n"
                             "```python\n"
                             "def hello_world():\n"
                             "    print(\"Hello, World!\")\n"
                             "    return True\n"
                             "```\n\n"
                             "JavaScript:\n"
                             "```javascript\n"
                             "const greet = (name) => {\n"
                             "  console.log(`Hello, ${name}!`);\n"
                             "};\n"
                             "```\n\n"
                             "C:\n"
                             "```c\n"
                             "#include <stdio.h>\n"
                             "int main() {\n"
                             "  printf(\"Hello!\\n\");\n"
                             "  return 0;\n"
                             "}\n"
                             "```\n\n"
                             "### Blockquotes\n"
                             "> This is a blockquote.\n"
                             "> It can span multiple lines.\n\n"
                             "### Links and Images\n"
                             "[Visit Example](https://example.com)\n\n"
                             "![Alt text](https://via.placeholder.com/150)\n\n"
                             "### Horizontal Rules\n"
                             "---\n\n"
                             "## Extended Markdown Syntax\n\n"
                             "### Tables\n"
                             "| Feature | Status |\n"
                             "|---------|--------|\n"
                             "| Tables  | ✓      |\n"
                             "| Lists   | ✓      |\n\n"
                             "### Task Lists\n"
                             "- [x] Completed task\n"
                             "- [ ] Pending task\n"
                             "- [ ] Another task\n\n"
                             "### Strikethrough & Highlight\n"
                             "~~Strikethrough text~~\n\n"
                             "==Highlighted text==\n\n"
                             "---\n\n"
                             "## Editor Features\n"
                             "- Fast editing with gap buffer\n"
                             "- Real-time syntax highlighting\n"
                             "- Live preview with scroll sync\n"
                             "- Full menu and toolbar\n"
                             "- Undo/Redo support\n"
                             "- Full screen mode\n"
                             "- **Complete Basic + Extended Markdown**\n\n"
                             "### Emoji Support :sparkles:\n"
                             "Use GitHub emoji shortcodes with colons!\\n\\n"
                             ":smile: :heart: :thumbsup: :tada: :rocket: :fire:\\n\\n"
                             ":wave: Hello! :sunglasses: Looking good! :100:\\n\\n"
                             "Emotions: :joy: :heart_eyes: :thinking: :cry:\\n\\n"
                             "Objects: :coffee: :pizza: :computer: :books:\\n\\n"
                             "Animals: :dog: :cat: :panda_face: :unicorn:\\n\\n"
                             "Nature: :sunny: :rainbow: :star: :snowflake:\\n\n";

        gtk_text_buffer_set_text(editor->text_buffer, welcome, -1);
        editor_update_preview(editor);
    }

    gtk_widget_show_all(editor->window);

    gtk_main();

    editor_free(editor);

    return 0;
}
