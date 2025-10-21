#include "gtk_editor.h"
#include "highlight.h"
#include <string.h>

static void on_text_changed(GtkTextBuffer *text_buffer, gpointer user_data);
static void on_new_clicked(GtkWidget *widget, gpointer user_data);
static void on_open_clicked(GtkWidget *widget, gpointer user_data);
static void on_save_clicked(GtkWidget *widget, gpointer user_data);
static void on_save_as_clicked(GtkWidget *widget, gpointer user_data);
static void on_undo_clicked(GtkWidget *widget, gpointer user_data);
static void on_redo_clicked(GtkWidget *widget, gpointer user_data);
static void on_cut_clicked(GtkWidget *widget, gpointer user_data);
static void on_copy_clicked(GtkWidget *widget, gpointer user_data);
static void on_paste_clicked(GtkWidget *widget, gpointer user_data);
static void on_select_all_clicked(GtkWidget *widget, gpointer user_data);
static void on_fullscreen_clicked(GtkWidget *widget, gpointer user_data);
static void on_exit_fullscreen_clicked(GtkWidget *widget, gpointer user_data);
static void on_editor_scroll_changed(GtkAdjustment *adjustment, gpointer user_data);
static void on_preview_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data);
static void apply_scroll_to_preview(Editor *editor);

Editor* editor_new(void) {
    Editor *editor = malloc(sizeof(Editor));
    if (!editor) return NULL;

    editor->gap_buffer = buffer_new();
    if (!editor->gap_buffer) {
        free(editor);
        return NULL;
    }

    editor->filename = NULL;
    editor->syncing_scroll = FALSE;
    editor->is_fullscreen = FALSE;
    editor->undo_stack = NULL;
    editor->redo_stack = NULL;
    editor->in_undo_redo = FALSE;
    editor->last_scroll_percent = 0.0;
    editor->scroll_timeout_id = 0;

    /* Create main window */
    editor->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(editor->window), "Markdown Editor");
    gtk_window_set_default_size(GTK_WINDOW(editor->window), 1200, 800);
    g_signal_connect(editor->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Create vertical box for layout */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(editor->window), vbox);

    /* Create menu bar */
    GtkWidget *menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    /* File menu */
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

    GtkWidget *new_menu_item = gtk_menu_item_new_with_label("New");
    g_signal_connect(new_menu_item, "activate", G_CALLBACK(on_new_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_menu_item);

    GtkWidget *open_menu_item = gtk_menu_item_new_with_label("Open");
    g_signal_connect(open_menu_item, "activate", G_CALLBACK(on_open_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_menu_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());

    GtkWidget *save_menu_item = gtk_menu_item_new_with_label("Save");
    g_signal_connect(save_menu_item, "activate", G_CALLBACK(on_save_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_menu_item);

    GtkWidget *save_as_menu_item = gtk_menu_item_new_with_label("Save as");
    g_signal_connect(save_as_menu_item, "activate", G_CALLBACK(on_save_as_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_menu_item);

    /* Edit menu */
    GtkWidget *edit_menu = gtk_menu_new();
    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit_item);

    GtkWidget *undo_menu_item = gtk_menu_item_new_with_label("Undo");
    g_signal_connect(undo_menu_item, "activate", G_CALLBACK(on_undo_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_menu_item);

    GtkWidget *redo_menu_item = gtk_menu_item_new_with_label("Redo");
    g_signal_connect(redo_menu_item, "activate", G_CALLBACK(on_redo_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_menu_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), gtk_separator_menu_item_new());

    GtkWidget *cut_menu_item = gtk_menu_item_new_with_label("Cut");
    g_signal_connect(cut_menu_item, "activate", G_CALLBACK(on_cut_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_menu_item);

    GtkWidget *copy_menu_item = gtk_menu_item_new_with_label("Copy");
    g_signal_connect(copy_menu_item, "activate", G_CALLBACK(on_copy_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_menu_item);

    GtkWidget *paste_menu_item = gtk_menu_item_new_with_label("Paste");
    g_signal_connect(paste_menu_item, "activate", G_CALLBACK(on_paste_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_menu_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), gtk_separator_menu_item_new());

    GtkWidget *select_all_menu_item = gtk_menu_item_new_with_label("Select all");
    g_signal_connect(select_all_menu_item, "activate", G_CALLBACK(on_select_all_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), select_all_menu_item);

    /* View menu */
    GtkWidget *view_menu = gtk_menu_new();
    GtkWidget *view_item = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_item);

    GtkWidget *fullscreen_menu_item = gtk_menu_item_new_with_label("Full screen");
    g_signal_connect(fullscreen_menu_item, "activate", G_CALLBACK(on_fullscreen_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), fullscreen_menu_item);

    GtkWidget *exit_fullscreen_menu_item = gtk_menu_item_new_with_label("Exit full screen");
    g_signal_connect(exit_fullscreen_menu_item, "activate", G_CALLBACK(on_exit_fullscreen_clicked), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), exit_fullscreen_menu_item);

    /* Create toolbar */
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    /* File toolbar items */
    GtkToolItem *new_button = gtk_tool_button_new(NULL, "New");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(new_button), "document-new");
    g_signal_connect(new_button, "clicked", G_CALLBACK(on_new_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_button, -1);

    GtkToolItem *open_button = gtk_tool_button_new(NULL, "Open");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(open_button), "document-open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_button, -1);

    GtkToolItem *save_button = gtk_tool_button_new(NULL, "Save");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save_button), "document-save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_button, -1);

    GtkToolItem *save_as_button = gtk_tool_button_new(NULL, "Save As");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save_as_button), "document-save-as");
    g_signal_connect(save_as_button, "clicked", G_CALLBACK(on_save_as_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_as_button, -1);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);

    /* Edit toolbar items */
    GtkToolItem *undo_button = gtk_tool_button_new(NULL, "Undo");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(undo_button), "edit-undo");
    g_signal_connect(undo_button, "clicked", G_CALLBACK(on_undo_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), undo_button, -1);

    GtkToolItem *redo_button = gtk_tool_button_new(NULL, "Redo");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(redo_button), "edit-redo");
    g_signal_connect(redo_button, "clicked", G_CALLBACK(on_redo_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), redo_button, -1);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);

    GtkToolItem *cut_button = gtk_tool_button_new(NULL, "Cut");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(cut_button), "edit-cut");
    g_signal_connect(cut_button, "clicked", G_CALLBACK(on_cut_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), cut_button, -1);

    GtkToolItem *copy_button = gtk_tool_button_new(NULL, "Copy");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(copy_button), "edit-copy");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), copy_button, -1);

    GtkToolItem *paste_button = gtk_tool_button_new(NULL, "Paste");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(paste_button), "edit-paste");
    g_signal_connect(paste_button, "clicked", G_CALLBACK(on_paste_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), paste_button, -1);

    GtkToolItem *select_all_button = gtk_tool_button_new(NULL, "Select All");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(select_all_button), "edit-select-all");
    g_signal_connect(select_all_button, "clicked", G_CALLBACK(on_select_all_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), select_all_button, -1);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);

    /* View toolbar items */
    GtkToolItem *fullscreen_button = gtk_tool_button_new(NULL, "Fullscreen");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(fullscreen_button), "view-fullscreen");
    g_signal_connect(fullscreen_button, "clicked", G_CALLBACK(on_fullscreen_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), fullscreen_button, -1);

    GtkToolItem *exit_fullscreen_button = gtk_tool_button_new(NULL, "Exit Fullscreen");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(exit_fullscreen_button), "view-restore");
    g_signal_connect(exit_fullscreen_button, "clicked", G_CALLBACK(on_exit_fullscreen_clicked), editor);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), exit_fullscreen_button, -1);

    /* Create horizontal paned for split view */
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    /* Create editor (left pane) */
    editor->editor_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor->editor_scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    editor->editor_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->editor_view), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(editor->editor_view), TRUE);
    gtk_container_add(GTK_CONTAINER(editor->editor_scroll), editor->editor_view);

    editor->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->editor_view));
    highlight_init_tags(editor->text_buffer);
    g_signal_connect(editor->text_buffer, "changed", G_CALLBACK(on_text_changed), editor);

    /* Connect scroll synchronization */
    GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(editor->editor_scroll));
    g_signal_connect(vadj, "value-changed", G_CALLBACK(on_editor_scroll_changed), editor);

    gtk_paned_pack1(GTK_PANED(paned), editor->editor_scroll, TRUE, TRUE);

    /* Create preview (right pane) using WebKit */
    GtkWidget *preview_scroll = gtk_scrolled_window_new(NULL, NULL);
    editor->preview_view = webkit_web_view_new();
    g_signal_connect(editor->preview_view, "load-changed", G_CALLBACK(on_preview_load_changed), editor);
    gtk_container_add(GTK_CONTAINER(preview_scroll), editor->preview_view);
    gtk_paned_pack2(GTK_PANED(paned), preview_scroll, TRUE, TRUE);

    /* Set initial split position */
    gtk_paned_set_position(GTK_PANED(paned), 600);

    return editor;
}

void editor_free(Editor *editor) {
    if (!editor) return;

    /* Free undo stack */
    while (editor->undo_stack) {
        UndoNode *next = editor->undo_stack->next;
        free(editor->undo_stack->text);
        free(editor->undo_stack);
        editor->undo_stack = next;
    }

    /* Free redo stack */
    while (editor->redo_stack) {
        UndoNode *next = editor->redo_stack->next;
        free(editor->redo_stack->text);
        free(editor->redo_stack);
        editor->redo_stack = next;
    }

    buffer_free(editor->gap_buffer);
    free(editor->filename);
    free(editor);
}

void editor_open_file(Editor *editor, const char *filename) {
    if (!editor || !filename) return;

    if (buffer_load_file(editor->gap_buffer, filename) == 0) {
        free(editor->filename);
        editor->filename = strdup(filename);
        if (!editor->filename) return;

        char *text = buffer_get_text(editor->gap_buffer);
        if (text) {
            gtk_text_buffer_set_text(editor->text_buffer, text, -1);
            free(text);
        }

        char title[512];
        snprintf(title, sizeof(title), "Markdown Editor - %s", filename);
        gtk_window_set_title(GTK_WINDOW(editor->window), title);
    }
}

void editor_save_file(Editor *editor) {
    if (!editor) return;

    const char *filename = editor->filename;
    if (!filename) {
        return;
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);

    if (text) {
        FILE *f = fopen(filename, "w");
        if (f) {
            fputs(text, f);
            fclose(f);

            char title[512];
            snprintf(title, sizeof(title), "Markdown Editor - %s", filename);
            gtk_window_set_title(GTK_WINDOW(editor->window), title);
        }
        g_free(text);
    }
}

void editor_save_as_file(Editor *editor) {
    on_save_as_clicked(NULL, editor);
}

void editor_update_preview(Editor *editor) {
    if (!editor) return;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    char *markdown = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);

    if (markdown) {
        char *html = highlight_markdown_to_html(markdown);
        if (html) {
            webkit_web_view_load_html(WEBKIT_WEB_VIEW(editor->preview_view), html, NULL);
            free(html);
        }
        g_free(markdown);
    }

    highlight_apply(editor->text_buffer);
}

static void on_text_changed(GtkTextBuffer *text_buffer, gpointer user_data) {
    Editor *editor = (Editor *)user_data;

    /* Don't save undo state during undo/redo operations */
    if (editor->in_undo_redo) {
        editor_update_preview(editor);
        return;
    }

    /* Save current state to undo stack */
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    if (text) {
        UndoNode *node = malloc(sizeof(UndoNode));
        if (node) {
            node->text = text;
            node->next = editor->undo_stack;
            editor->undo_stack = node;

            /* Clear redo stack when new text is entered */
            while (editor->redo_stack) {
                UndoNode *next = editor->redo_stack->next;
                g_free(editor->redo_stack->text);
                free(editor->redo_stack);
                editor->redo_stack = next;
            }
        } else {
            g_free(text);
        }
    }

    editor_update_preview(editor);
}

static void on_new_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    /* Clear the text buffer */
    gtk_text_buffer_set_text(editor->text_buffer, "", -1);

    /* Clear filename */
    free(editor->filename);
    editor->filename = NULL;

    /* Update window title */
    gtk_window_set_title(GTK_WINDOW(editor->window), "Markdown Editor - Untitled");

    /* Clear undo/redo stacks */
    while (editor->undo_stack) {
        UndoNode *next = editor->undo_stack->next;
        g_free(editor->undo_stack->text);
        free(editor->undo_stack);
        editor->undo_stack = next;
    }
    while (editor->redo_stack) {
        UndoNode *next = editor->redo_stack->next;
        g_free(editor->redo_stack->text);
        free(editor->redo_stack);
        editor->redo_stack = next;
    }

    /* Update the preview */
    editor_update_preview(editor);
}

static void on_open_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
                                                     GTK_WINDOW(editor->window),
                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Open", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Markdown files");
    gtk_file_filter_add_pattern(filter, "*.md");
    gtk_file_filter_add_pattern(filter, "*.markdown");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            editor_open_file(editor, filename);
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);
}

static void on_save_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    if (!editor->filename) {
        on_save_as_clicked(widget, user_data);
    } else {
        editor_save_file(editor);
    }
}

static void on_save_as_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save File",
                                                     GTK_WINDOW(editor->window),
                                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Save", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (editor->filename) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), editor->filename);
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            free(editor->filename);
            editor->filename = strdup(filename);
            g_free(filename);
            editor_save_file(editor);
        }
    }

    gtk_widget_destroy(dialog);
}

static void on_undo_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    if (!editor->undo_stack || !editor->undo_stack->next) return;

    /* Get current text and save to redo stack */
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    char *current_text = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);

    if (current_text) {
        UndoNode *redo_node = malloc(sizeof(UndoNode));
        if (redo_node) {
            redo_node->text = current_text;
            redo_node->next = editor->redo_stack;
            editor->redo_stack = redo_node;
        } else {
            g_free(current_text);
        }
    }

    /* Pop from undo stack and restore */
    UndoNode *undo_node = editor->undo_stack;
    editor->undo_stack = undo_node->next;

    editor->in_undo_redo = TRUE;
    gtk_text_buffer_set_text(editor->text_buffer, undo_node->text, -1);
    editor->in_undo_redo = FALSE;

    g_free(undo_node->text);
    free(undo_node);
}

static void on_redo_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    if (!editor->redo_stack) return;

    /* Get current text and save to undo stack */
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    char *current_text = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);

    if (current_text) {
        UndoNode *undo_node = malloc(sizeof(UndoNode));
        if (undo_node) {
            undo_node->text = current_text;
            undo_node->next = editor->undo_stack;
            editor->undo_stack = undo_node;
        } else {
            g_free(current_text);
        }
    }

    /* Pop from redo stack and restore */
    UndoNode *redo_node = editor->redo_stack;
    editor->redo_stack = redo_node->next;

    editor->in_undo_redo = TRUE;
    gtk_text_buffer_set_text(editor->text_buffer, redo_node->text, -1);
    editor->in_undo_redo = FALSE;

    g_free(redo_node->text);
    free(redo_node);
}

static void on_cut_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_cut_clipboard(editor->text_buffer, clipboard, TRUE);
}

static void on_copy_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_copy_clipboard(editor->text_buffer, clipboard);
}

static void on_paste_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_paste_clipboard(editor->text_buffer, clipboard, NULL, TRUE);
}

static void on_select_all_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    gtk_text_buffer_select_range(editor->text_buffer, &start, &end);
}

static void on_fullscreen_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    if (!editor->is_fullscreen) {
        gtk_window_fullscreen(GTK_WINDOW(editor->window));
        editor->is_fullscreen = TRUE;
    }
}

static void on_exit_fullscreen_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    Editor *editor = (Editor *)user_data;

    if (editor->is_fullscreen) {
        gtk_window_unfullscreen(GTK_WINDOW(editor->window));
        editor->is_fullscreen = FALSE;
    }
}

static void apply_scroll_to_preview(Editor *editor) {
    if (!editor) return;

    /* Scroll preview using JavaScript with better calculation */
    char script[512];
    snprintf(script, sizeof(script),
             "(function() {"
             "  var maxScroll = Math.max(0, document.documentElement.scrollHeight - window.innerHeight);"
             "  window.scrollTo(0, maxScroll * %.6f);"
             "})();",
             editor->last_scroll_percent);

    webkit_web_view_evaluate_javascript(WEBKIT_WEB_VIEW(editor->preview_view),
                                        script, -1, NULL, NULL, NULL, NULL, NULL);
}

static gboolean scroll_timeout_callback(gpointer user_data) {
    Editor *editor = (Editor *)user_data;
    editor->scroll_timeout_id = 0;

    apply_scroll_to_preview(editor);

    return G_SOURCE_REMOVE;
}

static void on_editor_scroll_changed(GtkAdjustment *adjustment, gpointer user_data) {
    Editor *editor = (Editor *)user_data;
    if (!editor || editor->syncing_scroll) return;

    /* Get scroll position as a percentage */
    gdouble value = gtk_adjustment_get_value(adjustment);
    gdouble upper = gtk_adjustment_get_upper(adjustment);
    gdouble page_size = gtk_adjustment_get_page_size(adjustment);

    /* Calculate scroll percentage (0.0 to 1.0) */
    editor->last_scroll_percent = 0.0;
    if (upper > page_size) {
        editor->last_scroll_percent = value / (upper - page_size);
    }

    /* Cancel existing timeout if any */
    if (editor->scroll_timeout_id > 0) {
        g_source_remove(editor->scroll_timeout_id);
    }

    /* Add a small delay to debounce rapid scroll events */
    editor->scroll_timeout_id = g_timeout_add(50, scroll_timeout_callback, editor);
}

static void on_preview_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data) {
    (void)web_view;
    Editor *editor = (Editor *)user_data;

    /* When page finishes loading, apply the saved scroll position */
    if (load_event == WEBKIT_LOAD_FINISHED) {
        /* Add a small delay to ensure the page is fully rendered */
        g_timeout_add(100, scroll_timeout_callback, editor);
    }
}
