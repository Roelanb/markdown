#include "highlight.h"
#include "emoji.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static GtkTextTag *tag_heading;
static GtkTextTag *tag_heading2;
static GtkTextTag *tag_bold;
static GtkTextTag *tag_italic;
static GtkTextTag *tag_code;
static GtkTextTag *tag_link;
static GtkTextTag *tag_blockquote;
static GtkTextTag *tag_list;
static GtkTextTag *tag_hr;
static GtkTextTag *tag_strikethrough;
static GtkTextTag *tag_table;
static GtkTextTag *tag_highlight;

void highlight_init_tags(GtkTextBuffer *buffer) {
    /* Heading 1-2 style */
    tag_heading = gtk_text_buffer_create_tag(buffer, "heading",
                                              "weight", PANGO_WEIGHT_BOLD,
                                              "scale", 1.8,
                                              "foreground", "#2E86AB",
                                              NULL);

    /* Heading 3-6 style */
    tag_heading2 = gtk_text_buffer_create_tag(buffer, "heading2",
                                               "weight", PANGO_WEIGHT_BOLD,
                                               "scale", 1.3,
                                               "foreground", "#2E86AB",
                                               NULL);

    /* Bold style */
    tag_bold = gtk_text_buffer_create_tag(buffer, "bold",
                                          "weight", PANGO_WEIGHT_BOLD,
                                          NULL);

    /* Italic style */
    tag_italic = gtk_text_buffer_create_tag(buffer, "italic",
                                            "style", PANGO_STYLE_ITALIC,
                                            NULL);

    /* Code style */
    tag_code = gtk_text_buffer_create_tag(buffer, "code",
                                          "family", "monospace",
                                          "background", "#F5F5F5",
                                          "foreground", "#D84315",
                                          NULL);

    /* Link style */
    tag_link = gtk_text_buffer_create_tag(buffer, "link",
                                          "foreground", "#1976D2",
                                          "underline", PANGO_UNDERLINE_SINGLE,
                                          NULL);

    /* Blockquote style */
    tag_blockquote = gtk_text_buffer_create_tag(buffer, "blockquote",
                                                "foreground", "#555555",
                                                "style", PANGO_STYLE_ITALIC,
                                                "left-margin", 20,
                                                NULL);

    /* List style */
    tag_list = gtk_text_buffer_create_tag(buffer, "list",
                                          "foreground", "#006400",
                                          NULL);

    /* Horizontal rule style */
    tag_hr = gtk_text_buffer_create_tag(buffer, "hr",
                                        "foreground", "#CCCCCC",
                                        NULL);

    /* Strikethrough style */
    tag_strikethrough = gtk_text_buffer_create_tag(buffer, "strikethrough",
                                                   "strikethrough", TRUE,
                                                   "foreground", "#888888",
                                                   NULL);

    /* Table style */
    tag_table = gtk_text_buffer_create_tag(buffer, "table",
                                           "foreground", "#008080",
                                           "family", "monospace",
                                           NULL);

    /* Highlight style */
    tag_highlight = gtk_text_buffer_create_tag(buffer, "highlight",
                                               "background", "#FFFF00",
                                               NULL);
}

void highlight_apply(GtkTextBuffer *buffer) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);

    /* Remove all existing tags */
    gtk_text_buffer_remove_all_tags(buffer, &start, &end);

    GtkTextIter line_start = start;
    GtkTextIter line_end;

    /* Process each line */
    while (!gtk_text_iter_is_end(&line_start)) {
        line_end = line_start;
        gtk_text_iter_forward_to_line_end(&line_end);

        char *line = gtk_text_buffer_get_text(buffer, &line_start, &line_end, FALSE);
        if (!line) break;

        int len = strlen(line);
        int line_handled = 0;

        /* Check for heading */
        if (len > 0 && line[0] == '#') {
            int level = 0;
            while (level < len && line[level] == '#') level++;
            if (level <= 6 && (level == len || line[level] == ' ')) {
                if (level <= 2) {
                    gtk_text_buffer_apply_tag(buffer, tag_heading, &line_start, &line_end);
                } else {
                    gtk_text_buffer_apply_tag(buffer, tag_heading2, &line_start, &line_end);
                }
                line_handled = 1;
            }
        }

        /* Check for blockquote */
        if (!line_handled && len > 0 && line[0] == '>') {
            gtk_text_buffer_apply_tag(buffer, tag_blockquote, &line_start, &line_end);
            line_handled = 1;
        }

        /* Check for task list (extended syntax) */
        if (!line_handled && len >= 5 && line[0] == '-' && line[1] == ' ' &&
            line[2] == '[' && (line[3] == ' ' || line[3] == 'x' || line[3] == 'X') && line[4] == ']') {
            gtk_text_buffer_apply_tag(buffer, tag_list, &line_start, &line_end);
            line_handled = 1;
        }

        /* Check for unordered list */
        if (!line_handled && len > 0 && (line[0] == '-' || line[0] == '*' || line[0] == '+')) {
            if (len == 1 || line[1] == ' ') {
                gtk_text_buffer_apply_tag(buffer, tag_list, &line_start, &line_end);
                line_handled = 1;
            }
        }

        /* Check for ordered list */
        if (!line_handled && len > 0 && isdigit(line[0])) {
            int i = 0;
            while (i < len && isdigit(line[i])) i++;
            if (i < len && line[i] == '.' && (i + 1 == len || line[i + 1] == ' ')) {
                gtk_text_buffer_apply_tag(buffer, tag_list, &line_start, &line_end);
                line_handled = 1;
            }
        }

        /* Check for table (extended syntax) */
        if (!line_handled && len > 0 && line[0] == '|') {
            gtk_text_buffer_apply_tag(buffer, tag_table, &line_start, &line_end);
            line_handled = 1;
        }

        /* Check for horizontal rule */
        if (!line_handled && len >= 3) {
            int dash_count = 0, star_count = 0, under_count = 0;
            for (int i = 0; i < len; i++) {
                if (line[i] == '-') dash_count++;
                else if (line[i] == '*') star_count++;
                else if (line[i] == '_') under_count++;
                else if (line[i] != ' ') break;
            }
            if (dash_count >= 3 || star_count >= 3 || under_count >= 3) {
                gtk_text_buffer_apply_tag(buffer, tag_hr, &line_start, &line_end);
                line_handled = 1;
            }
        }

        /* Check for inline formatting if line not handled as special */
        if (!line_handled) {
            for (int i = 0; i < len; i++) {
                /* Bold and italic ***text*** */
                if (i + 2 < len && line[i] == '*' && line[i + 1] == '*' && line[i + 2] == '*') {
                    int j = i + 3;
                    while (j + 2 < len && !(line[j] == '*' && line[j + 1] == '*' && line[j + 2] == '*')) {
                        j++;
                    }
                    if (j + 2 < len) {
                        GtkTextIter bold_start = line_start;
                        GtkTextIter bold_end = line_start;
                        gtk_text_iter_forward_chars(&bold_start, i);
                        gtk_text_iter_forward_chars(&bold_end, j + 3);
                        gtk_text_buffer_apply_tag(buffer, tag_bold, &bold_start, &bold_end);
                        gtk_text_buffer_apply_tag(buffer, tag_italic, &bold_start, &bold_end);
                        i = j + 2;
                    }
                }
                /* Bold **text** or __text__ */
                else if ((i + 1 < len && line[i] == '*' && line[i + 1] == '*') ||
                         (i + 1 < len && line[i] == '_' && line[i + 1] == '_')) {
                    char marker = line[i];
                    int j = i + 2;
                    while (j + 1 < len && !(line[j] == marker && line[j + 1] == marker)) {
                        j++;
                    }
                    if (j + 1 < len) {
                        GtkTextIter bold_start = line_start;
                        GtkTextIter bold_end = line_start;
                        gtk_text_iter_forward_chars(&bold_start, i);
                        gtk_text_iter_forward_chars(&bold_end, j + 2);
                        gtk_text_buffer_apply_tag(buffer, tag_bold, &bold_start, &bold_end);
                        i = j + 1;
                    }
                }
                /* Italic *text* or _text_ */
                else if ((line[i] == '*' || line[i] == '_') &&
                         (i == 0 || (line[i - 1] != '*' && line[i - 1] != '_'))) {
                    char marker = line[i];
                    int j = i + 1;
                    while (j < len && line[j] != marker) {
                        j++;
                    }
                    if (j < len && (j + 1 >= len || (line[j + 1] != '*' && line[j + 1] != '_'))) {
                        GtkTextIter italic_start = line_start;
                        GtkTextIter italic_end = line_start;
                        gtk_text_iter_forward_chars(&italic_start, i);
                        gtk_text_iter_forward_chars(&italic_end, j + 1);
                        gtk_text_buffer_apply_tag(buffer, tag_italic, &italic_start, &italic_end);
                        i = j;
                    }
                }
                /* Strikethrough ~~text~~ (extended syntax) */
                else if (i + 1 < len && line[i] == '~' && line[i + 1] == '~') {
                    int j = i + 2;
                    while (j + 1 < len && !(line[j] == '~' && line[j + 1] == '~')) {
                        j++;
                    }
                    if (j + 1 < len) {
                        GtkTextIter strike_start = line_start;
                        GtkTextIter strike_end = line_start;
                        gtk_text_iter_forward_chars(&strike_start, i);
                        gtk_text_iter_forward_chars(&strike_end, j + 2);
                        gtk_text_buffer_apply_tag(buffer, tag_strikethrough, &strike_start, &strike_end);
                        i = j + 1;
                    }
                }
                /* Highlight ==text== (extended syntax) */
                else if (i + 1 < len && line[i] == '=' && line[i + 1] == '=') {
                    int j = i + 2;
                    while (j + 1 < len && !(line[j] == '=' && line[j + 1] == '=')) {
                        j++;
                    }
                    if (j + 1 < len) {
                        GtkTextIter hl_start = line_start;
                        GtkTextIter hl_end = line_start;
                        gtk_text_iter_forward_chars(&hl_start, i);
                        gtk_text_iter_forward_chars(&hl_end, j + 2);
                        gtk_text_buffer_apply_tag(buffer, tag_highlight, &hl_start, &hl_end);
                        i = j + 1;
                    }
                }
                /* Code `text` */
                else if (line[i] == '`') {
                    int j = i + 1;
                    while (j < len && line[j] != '`') {
                        j++;
                    }
                    if (j < len) {
                        GtkTextIter code_start = line_start;
                        GtkTextIter code_end = line_start;
                        gtk_text_iter_forward_chars(&code_start, i);
                        gtk_text_iter_forward_chars(&code_end, j + 1);
                        gtk_text_buffer_apply_tag(buffer, tag_code, &code_start, &code_end);
                        i = j;
                    }
                }
                /* Link [text](url) and Image ![alt](url) */
                else if (line[i] == '[' || (line[i] == '!' && i + 1 < len && line[i + 1] == '[')) {
                    int start_pos = (line[i] == '!') ? i : i;
                    int bracket_start = (line[i] == '!') ? i + 1 : i;
                    int j = bracket_start + 1;
                    while (j < len && line[j] != ']') {
                        j++;
                    }
                    if (j + 1 < len && line[j + 1] == '(') {
                        int k = j + 2;
                        while (k < len && line[k] != ')') {
                            k++;
                        }
                        if (k < len) {
                            GtkTextIter link_start = line_start;
                            GtkTextIter link_end = line_start;
                            gtk_text_iter_forward_chars(&link_start, start_pos);
                            gtk_text_iter_forward_chars(&link_end, k + 1);
                            gtk_text_buffer_apply_tag(buffer, tag_link, &link_start, &link_end);
                            i = k;
                        }
                    }
                }
            }
        }

        g_free(line);
        gtk_text_iter_forward_line(&line_start);
    }
}

char* highlight_markdown_to_html(const char *markdown) {
    if (!markdown) return NULL;

    size_t len = strlen(markdown);
    size_t html_capacity = len * 4 + 2048;
    char *html = malloc(html_capacity);
    if (!html) return NULL;

    strcpy(html, "<!DOCTYPE html><html><head>");
    strcat(html, "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/github.min.css'>");
    strcat(html, "<script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js'></script>");
    strcat(html, "<script>hljs.highlightAll();</script>");
    strcat(html, "<style>");
    strcat(html, "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ");
    strcat(html, "padding: 20px; max-width: 800px; margin: 0 auto; line-height: 1.6; color: #333; }");
    strcat(html, "h1, h2, h3, h4, h5, h6 { color: #2E86AB; margin-top: 24px; margin-bottom: 16px; font-weight: 600; }");
    strcat(html, "h1 { font-size: 2em; border-bottom: 1px solid #eaecef; padding-bottom: 0.3em; }");
    strcat(html, "h2 { font-size: 1.5em; border-bottom: 1px solid #eaecef; padding-bottom: 0.3em; }");
    strcat(html, "code { background: #F5F5F5; padding: 2px 6px; border-radius: 3px; color: #D84315; font-family: monospace; font-size: 85%; }");
    strcat(html, "pre { background: #F5F5F5; padding: 16px; border-radius: 6px; overflow-x: auto; margin: 16px 0; }");
    strcat(html, "pre code { background: none; padding: 0; color: inherit; display: block; }");
    strcat(html, "pre code.hljs { background: #f6f8fa; }");
    strcat(html, "a { color: #1976D2; text-decoration: none; }");
    strcat(html, "a:hover { text-decoration: underline; }");
    strcat(html, "blockquote { border-left: 4px solid #ddd; margin: 0; padding-left: 16px; color: #555; }");
    strcat(html, "ul, ol { padding-left: 2em; }");
    strcat(html, "li { margin: 0.25em 0; }");
    strcat(html, "hr { border: none; border-top: 2px solid #eaecef; margin: 24px 0; }");
    strcat(html, "img { max-width: 100%; height: auto; }");
    strcat(html, "p { margin: 16px 0; }");
    strcat(html, "del { color: #888; }");
    strcat(html, "mark { background: #ffff00; padding: 2px 4px; }");
    strcat(html, "table { border-collapse: collapse; width: 100%; margin: 16px 0; }");
    strcat(html, "th, td { border: 1px solid #ddd; padding: 8px 12px; text-align: left; }");
    strcat(html, "th { background-color: #f5f5f5; font-weight: 600; }");
    strcat(html, "tr:nth-child(even) { background-color: #fafafa; }");
    strcat(html, "input[type=checkbox] { margin-right: 8px; }");
    strcat(html, ".footnote { font-size: 0.85em; color: #666; vertical-align: super; }");
    strcat(html, ".footnotes { margin-top: 32px; border-top: 1px solid #eee; padding-top: 16px; font-size: 0.9em; }");
    strcat(html, "</style></head><body>");

    size_t html_pos = strlen(html);
    int in_code_block = 0;
    int in_list = 0;
    int in_blockquote = 0;
    int in_paragraph = 0;
    int i = 0;

    while (i < (int)len) {
        /* Ensure we don't overflow */
        if (html_pos + 512 > html_capacity) {
            html_capacity *= 2;
            char *new_html = realloc(html, html_capacity);
            if (!new_html) {
                free(html);
                return NULL;
            }
            html = new_html;
        }

        /* Code block ``` */
        if (i + 2 < (int)len && markdown[i] == '`' && markdown[i + 1] == '`' && markdown[i + 2] == '`') {
            if (in_paragraph) {
                strcat(html, "</p>");
                html_pos = strlen(html);
                in_paragraph = 0;
            }
            if (in_code_block) {
                strcat(html, "</code></pre>");
                html_pos = strlen(html);
                in_code_block = 0;
            } else {
                i += 3;
                /* Extract language specifier */
                char lang[32] = {0};
                int lang_len = 0;
                while (i < (int)len && markdown[i] != '\n' && lang_len < 31) {
                    if (markdown[i] != ' ' && markdown[i] != '\t') {
                        lang[lang_len++] = markdown[i];
                    }
                    i++;
                }
                lang[lang_len] = '\0';

                /* Start code block with language class */
                if (lang_len > 0) {
                    strcat(html, "<pre><code class='language-");
                    strcat(html, lang);
                    strcat(html, "'>");
                } else {
                    strcat(html, "<pre><code>");
                }
                html_pos = strlen(html);
                in_code_block = 1;
                i++;
                continue;
            }
            i += 3;
            while (i < (int)len && markdown[i] != '\n') i++;
            i++;
            continue;
        }

        /* Process line-based elements */
        if (!in_code_block && (i == 0 || markdown[i - 1] == '\n')) {
            /* Headings */
            int level = 0;
            while (i + level < (int)len && markdown[i + level] == '#') level++;
            if (level > 0 && level <= 6 && i + level < (int)len && markdown[i + level] == ' ') {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (in_list) {
                    strcat(html, "</ul>");
                    html_pos = strlen(html);
                    in_list = 0;
                }
                if (in_blockquote) {
                    strcat(html, "</blockquote>");
                    html_pos = strlen(html);
                    in_blockquote = 0;
                }

                char tag[16];
                snprintf(tag, sizeof(tag), "<h%d>", level);
                strcat(html, tag);
                html_pos = strlen(html);
                i += level + 1;

                while (i < (int)len && markdown[i] != '\n') {
                    if (markdown[i] == '<') strcat(html, "&lt;");
                    else if (markdown[i] == '>') strcat(html, "&gt;");
                    else if (markdown[i] == '&') strcat(html, "&amp;");
                    else { html[html_pos++] = markdown[i]; html[html_pos] = '\0'; }
                    i++;
                }

                snprintf(tag, sizeof(tag), "</h%d>", level);
                strcat(html, tag);
                html_pos = strlen(html);
                continue;
            }

            /* Table (extended syntax) */
            if (markdown[i] == '|') {
                /* Simple table support - detect table rows */
                int is_header_separator = 0;

                /* Check if next line is header separator (|---|---|) */
                int next_line = i;
                while (next_line < (int)len && markdown[next_line] != '\n') next_line++;
                if (next_line + 1 < (int)len) {
                    int check = next_line + 1;
                    if (markdown[check] == '|') {
                        is_header_separator = 1;
                        while (check < (int)len && markdown[check] != '\n') {
                            if (markdown[check] != '|' && markdown[check] != '-' &&
                                markdown[check] != ':' && markdown[check] != ' ') {
                                is_header_separator = 0;
                                break;
                            }
                            check++;
                        }
                    }
                }

                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }

                strcat(html, "<table><thead><tr><th>");
                html_pos = strlen(html);
                i++; /* Skip initial | */

                /* Parse header cells */
                while (i < (int)len && markdown[i] != '\n') {
                    if (markdown[i] == '|') {
                        /* Check if this is the trailing pipe */
                        int j = i + 1;
                        while (j < (int)len && markdown[j] == ' ') j++;
                        if (j < (int)len && markdown[j] != '\n') {
                            strcat(html, "</th><th>");
                            html_pos = strlen(html);
                        }
                        i++;
                    } else if (markdown[i] != ' ' || html[html_pos - 1] != '>') {
                        if (html[html_pos - 1] == 'h' && html[html_pos - 2] == 't') {
                            /* Just started a cell, skip leading space */
                            if (markdown[i] != ' ') {
                                html[html_pos++] = markdown[i];
                                html[html_pos] = '\0';
                            }
                        } else {
                            html[html_pos++] = markdown[i];
                            html[html_pos] = '\0';
                        }
                        i++;
                    } else {
                        i++;
                    }
                }
                strcat(html, "</th></tr></thead>");
                html_pos = strlen(html);
                i++;

                /* Skip separator line if present */
                if (is_header_separator && i < (int)len && markdown[i] == '|') {
                    while (i < (int)len && markdown[i] != '\n') i++;
                    i++;
                }

                /* Parse table body */
                strcat(html, "<tbody>");
                html_pos = strlen(html);
                while (i < (int)len && markdown[i] == '|') {
                    strcat(html, "<tr><td>");
                    html_pos = strlen(html);
                    i++;

                    while (i < (int)len && markdown[i] != '\n') {
                        if (markdown[i] == '|') {
                            /* Check if this is the trailing pipe */
                            int j = i + 1;
                            while (j < (int)len && markdown[j] == ' ') j++;
                            if (j < (int)len && markdown[j] != '\n') {
                                strcat(html, "</td><td>");
                                html_pos = strlen(html);
                            }
                            i++;
                        } else if (markdown[i] != ' ' || html[html_pos - 1] != '>') {
                            if (html[html_pos - 1] == 'd' && html[html_pos - 2] == 't') {
                                if (markdown[i] != ' ') {
                                    html[html_pos++] = markdown[i];
                                    html[html_pos] = '\0';
                                }
                            } else {
                                html[html_pos++] = markdown[i];
                                html[html_pos] = '\0';
                            }
                            i++;
                        } else {
                            i++;
                        }
                    }
                    strcat(html, "</td></tr>");
                    html_pos = strlen(html);
                    i++;
                }
                strcat(html, "</tbody></table>");
                html_pos = strlen(html);
                continue;
            }

            /* Blockquote */
            if (markdown[i] == '>') {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (in_list) {
                    strcat(html, "</ul>");
                    html_pos = strlen(html);
                    in_list = 0;
                }
                if (!in_blockquote) {
                    strcat(html, "<blockquote>");
                    html_pos = strlen(html);
                    in_blockquote = 1;
                }
                i++;
                if (i < (int)len && markdown[i] == ' ') i++;
                continue;
            } else if (in_blockquote && markdown[i] == '\n') {
                strcat(html, "</blockquote>");
                html_pos = strlen(html);
                in_blockquote = 0;
                i++;
                continue;
            }

            /* Task list (extended syntax) - [ ] or - [x] */
            if (markdown[i] == '-' && i + 5 < (int)len && markdown[i + 1] == ' ' &&
                markdown[i + 2] == '[' && (markdown[i + 3] == ' ' || markdown[i + 3] == 'x' || markdown[i + 3] == 'X') &&
                markdown[i + 4] == ']' && markdown[i + 5] == ' ') {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (!in_list) {
                    strcat(html, "<ul style='list-style: none;'>");
                    html_pos = strlen(html);
                    in_list = 1;
                }
                strcat(html, "<li><input type='checkbox' disabled ");
                html_pos = strlen(html);
                if (markdown[i + 3] == 'x' || markdown[i + 3] == 'X') {
                    strcat(html, "checked");
                    html_pos = strlen(html);
                }
                strcat(html, ">");
                html_pos = strlen(html);
                i += 6;
                continue;
            }

            /* Unordered list */
            if ((markdown[i] == '-' || markdown[i] == '*' || markdown[i] == '+') &&
                (i + 1 < (int)len && markdown[i + 1] == ' ')) {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (!in_list) {
                    strcat(html, "<ul>");
                    html_pos = strlen(html);
                    in_list = 1;
                }
                strcat(html, "<li>");
                html_pos = strlen(html);
                i += 2;
                continue;
            }

            /* Ordered list */
            int num_start = i;
            int is_ordered = 0;
            while (i < (int)len && isdigit(markdown[i])) i++;
            if (i > num_start && i < (int)len && markdown[i] == '.' &&
                i + 1 < (int)len && markdown[i + 1] == ' ') {
                is_ordered = 1;
            } else {
                i = num_start;
            }

            if (is_ordered) {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (in_list != 2) {
                    if (in_list == 1) {
                        strcat(html, "</ul>");
                        html_pos = strlen(html);
                    }
                    strcat(html, "<ol>");
                    html_pos = strlen(html);
                    in_list = 2;
                }
                strcat(html, "<li>");
                html_pos = strlen(html);
                i += 2;
                continue;
            }

            /* Horizontal rule */
            int dash_count = 0, star_count = 0, under_count = 0;
            int temp_i = i;
            while (temp_i < (int)len && markdown[temp_i] != '\n') {
                if (markdown[temp_i] == '-') dash_count++;
                else if (markdown[temp_i] == '*') star_count++;
                else if (markdown[temp_i] == '_') under_count++;
                else if (markdown[temp_i] != ' ') break;
                temp_i++;
            }
            if ((dash_count >= 3 || star_count >= 3 || under_count >= 3) &&
                (temp_i >= (int)len || markdown[temp_i] == '\n')) {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                if (in_list) {
                    if (in_list == 1) strcat(html, "</ul>");
                    else strcat(html, "</ol>");
                    html_pos = strlen(html);
                    in_list = 0;
                }
                strcat(html, "<hr>");
                html_pos = strlen(html);
                i = temp_i;
                if (i < (int)len) i++;
                continue;
            }
        }

        /* Handle list item end */
        if (in_list && markdown[i] == '\n') {
            if (i + 1 < (int)len && markdown[i + 1] != '\n' &&
                markdown[i + 1] != '-' && markdown[i + 1] != '*' &&
                markdown[i + 1] != '+' && !isdigit(markdown[i + 1])) {
                strcat(html, "</li>");
                html_pos = strlen(html);
            } else if (i + 1 >= (int)len || markdown[i + 1] == '\n') {
                strcat(html, "</li>");
                if (in_list == 1) strcat(html, "</ul>");
                else strcat(html, "</ol>");
                html_pos = strlen(html);
                in_list = 0;
            }
        }

        /* Emoji :shortcode: */
        if (!in_code_block && markdown[i] == ':') {
            /* Try to find the closing : */
            int emoji_end = i + 1;
            char shortcode[64] = {0};
            int shortcode_len = 0;

            /* Build the shortcode including both colons */
            shortcode[shortcode_len++] = ':';
            while (emoji_end < (int)len && markdown[emoji_end] != ':' &&
                   markdown[emoji_end] != ' ' && markdown[emoji_end] != '\n' &&
                   shortcode_len < 62) {
                shortcode[shortcode_len++] = markdown[emoji_end];
                emoji_end++;
            }

            /* If we found a closing colon, complete the shortcode and look it up */
            if (emoji_end < (int)len && markdown[emoji_end] == ':') {
                shortcode[shortcode_len++] = ':';
                shortcode[shortcode_len] = '\0';

                const char *emoji_unicode = emoji_find(shortcode);
                if (emoji_unicode) {
                    /* Found a valid emoji - insert the unicode character */
                    strcat(html, emoji_unicode);
                    html_pos = strlen(html);
                    i = emoji_end + 1;
                    continue;
                }
            }
            /* If no emoji found, fall through to normal character handling */
        }

        /* Strikethrough ~~text~~ (extended syntax) */
        if (!in_code_block && i + 1 < (int)len && markdown[i] == '~' && markdown[i + 1] == '~') {
            strcat(html, "<del>");
            html_pos = strlen(html);
            i += 2;
            while (i + 1 < (int)len && !(markdown[i] == '~' && markdown[i + 1] == '~')) {
                html[html_pos++] = markdown[i++];
            }
            html[html_pos] = '\0';
            strcat(html, "</del>");
            html_pos = strlen(html);
            if (i + 1 < (int)len) i += 2;
            continue;
        }

        /* Highlight ==text== (extended syntax) */
        if (!in_code_block && i + 1 < (int)len && markdown[i] == '=' && markdown[i + 1] == '=') {
            strcat(html, "<mark>");
            html_pos = strlen(html);
            i += 2;
            while (i + 1 < (int)len && !(markdown[i] == '=' && markdown[i + 1] == '=')) {
                html[html_pos++] = markdown[i++];
            }
            html[html_pos] = '\0';
            strcat(html, "</mark>");
            html_pos = strlen(html);
            if (i + 1 < (int)len) i += 2;
            continue;
        }

        /* Bold ***text*** and italic combined */
        if (!in_code_block && i + 2 < (int)len && markdown[i] == '*' &&
            markdown[i + 1] == '*' && markdown[i + 2] == '*') {
            strcat(html, "<strong><em>");
            html_pos = strlen(html);
            i += 3;
            while (i + 2 < (int)len && !(markdown[i] == '*' && markdown[i + 1] == '*' && markdown[i + 2] == '*')) {
                html[html_pos++] = markdown[i++];
            }
            html[html_pos] = '\0';
            strcat(html, "</em></strong>");
            html_pos = strlen(html);
            if (i + 2 < (int)len) i += 3;
            continue;
        }

        /* Bold **text** or __text__ */
        if (!in_code_block && ((i + 1 < (int)len && markdown[i] == '*' && markdown[i + 1] == '*') ||
                                (i + 1 < (int)len && markdown[i] == '_' && markdown[i + 1] == '_'))) {
            char marker = markdown[i];
            strcat(html, "<strong>");
            html_pos = strlen(html);
            i += 2;
            while (i + 1 < (int)len && !(markdown[i] == marker && markdown[i + 1] == marker)) {
                html[html_pos++] = markdown[i++];
            }
            html[html_pos] = '\0';
            strcat(html, "</strong>");
            html_pos = strlen(html);
            if (i + 1 < (int)len) i += 2;
            continue;
        }

        /* Italic *text* or _text_ */
        if (!in_code_block && (markdown[i] == '*' || markdown[i] == '_') &&
            (i == 0 || (markdown[i - 1] != '*' && markdown[i - 1] != '_'))) {
            char marker = markdown[i];
            int j = i + 1;
            while (j < (int)len && markdown[j] != marker && markdown[j] != '\n') j++;
            if (j < (int)len && markdown[j] == marker) {
                strcat(html, "<em>");
                html_pos = strlen(html);
                i++;
                while (i < j) {
                    html[html_pos++] = markdown[i++];
                }
                html[html_pos] = '\0';
                strcat(html, "</em>");
                html_pos = strlen(html);
                i++;
                continue;
            }
        }

        /* Inline code `text` */
        if (!in_code_block && markdown[i] == '`') {
            strcat(html, "<code>");
            html_pos = strlen(html);
            i++;
            while (i < (int)len && markdown[i] != '`') {
                if (markdown[i] == '<') strcat(html, "&lt;");
                else if (markdown[i] == '>') strcat(html, "&gt;");
                else if (markdown[i] == '&') strcat(html, "&amp;");
                else { html[html_pos++] = markdown[i]; html[html_pos] = '\0'; }
                i++;
            }
            strcat(html, "</code>");
            html_pos = strlen(html);
            if (i < (int)len) i++;
            continue;
        }

        /* Image ![alt](url) */
        if (!in_code_block && markdown[i] == '!' && i + 1 < (int)len && markdown[i + 1] == '[') {
            int j = i + 2;
            while (j < (int)len && markdown[j] != ']') j++;
            if (j < (int)len && j + 1 < (int)len && markdown[j + 1] == '(') {
                int k = j + 2;
                while (k < (int)len && markdown[k] != ')') k++;
                if (k < (int)len) {
                    strcat(html, "<img src=\"");
                    html_pos = strlen(html);
                    for (int m = j + 2; m < k; m++) {
                        html[html_pos++] = markdown[m];
                    }
                    html[html_pos] = '\0';
                    strcat(html, "\" alt=\"");
                    html_pos = strlen(html);
                    for (int m = i + 2; m < j; m++) {
                        html[html_pos++] = markdown[m];
                    }
                    html[html_pos] = '\0';
                    strcat(html, "\">");
                    html_pos = strlen(html);
                    i = k + 1;
                    continue;
                }
            }
        }

        /* Link [text](url) */
        if (!in_code_block && markdown[i] == '[') {
            int j = i + 1;
            while (j < (int)len && markdown[j] != ']') j++;
            if (j < (int)len && j + 1 < (int)len && markdown[j + 1] == '(') {
                int k = j + 2;
                while (k < (int)len && markdown[k] != ')') k++;
                if (k < (int)len) {
                    strcat(html, "<a href=\"");
                    html_pos = strlen(html);
                    for (int m = j + 2; m < k; m++) {
                        html[html_pos++] = markdown[m];
                    }
                    html[html_pos] = '\0';
                    strcat(html, "\">");
                    html_pos = strlen(html);
                    for (int m = i + 1; m < j; m++) {
                        html[html_pos++] = markdown[m];
                    }
                    html[html_pos] = '\0';
                    strcat(html, "</a>");
                    html_pos = strlen(html);
                    i = k + 1;
                    continue;
                }
            }
        }

        /* Paragraph handling */
        if (!in_code_block && !in_list && !in_blockquote && markdown[i] == '\n') {
            if (i + 1 < (int)len && markdown[i + 1] == '\n') {
                if (in_paragraph) {
                    strcat(html, "</p>");
                    html_pos = strlen(html);
                    in_paragraph = 0;
                }
                i += 2;
                continue;
            } else {
                strcat(html, " ");
                html_pos = strlen(html);
                i++;
                continue;
            }
        } else if (!in_code_block && !in_list && !in_blockquote && !in_paragraph &&
                   markdown[i] != '\n' && markdown[i] != ' ') {
            strcat(html, "<p>");
            html_pos = strlen(html);
            in_paragraph = 1;
        }

        /* Regular character */
        if (markdown[i] == '<') {
            strcat(html, "&lt;");
            html_pos = strlen(html);
        } else if (markdown[i] == '>') {
            strcat(html, "&gt;");
            html_pos = strlen(html);
        } else if (markdown[i] == '&') {
            strcat(html, "&amp;");
            html_pos = strlen(html);
        } else {
            html[html_pos++] = markdown[i];
            html[html_pos] = '\0';
        }
        i++;
    }

    /* Close any open tags */
    if (in_paragraph) strcat(html, "</p>");
    if (in_list == 1) strcat(html, "</ul>");
    if (in_list == 2) strcat(html, "</ol>");
    if (in_blockquote) strcat(html, "</blockquote>");
    if (in_code_block) strcat(html, "</code></pre>");

    strcat(html, "</body></html>");
    return html;
}
