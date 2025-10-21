#ifndef GTK_EDITOR_H
#define GTK_EDITOR_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "buffer.h"

typedef struct UndoNode {
    char *text;
    struct UndoNode *next;
} UndoNode;

typedef struct {
    GtkWidget *window;
    GtkWidget *editor_view;
    GtkWidget *editor_scroll;
    GtkWidget *preview_view;
    GtkTextBuffer *text_buffer;
    GapBuffer *gap_buffer;
    char *filename;
    gboolean syncing_scroll;
    gboolean is_fullscreen;
    UndoNode *undo_stack;
    UndoNode *redo_stack;
    gboolean in_undo_redo;
    gdouble last_scroll_percent;
    guint scroll_timeout_id;
} Editor;

/* Create and initialize the editor */
Editor* editor_new(void);

/* Free editor resources */
void editor_free(Editor *editor);

/* Open file in editor */
void editor_open_file(Editor *editor, const char *filename);

/* Save editor content to file */
void editor_save_file(Editor *editor);

/* Save editor content to new file */
void editor_save_as_file(Editor *editor);

/* Update preview from editor content */
void editor_update_preview(Editor *editor);

#endif
