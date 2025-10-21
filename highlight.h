#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <gtk/gtk.h>

/* Initialize syntax highlighting tags for GTK TextBuffer */
void highlight_init_tags(GtkTextBuffer *buffer);

/* Apply markdown syntax highlighting to buffer */
void highlight_apply(GtkTextBuffer *buffer);

/* Convert markdown to HTML for preview */
char* highlight_markdown_to_html(const char *markdown);

#endif
