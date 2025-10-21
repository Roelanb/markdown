#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 1024
#define GAP_SIZE 256

GapBuffer* buffer_new(void) {
    GapBuffer *buf = malloc(sizeof(GapBuffer));
    if (!buf) return NULL;

    buf->capacity = INITIAL_CAPACITY;
    buf->text = malloc(buf->capacity);
    if (!buf->text) {
        free(buf);
        return NULL;
    }

    buf->gap_start = 0;
    buf->gap_end = buf->capacity;

    return buf;
}

void buffer_free(GapBuffer *buf) {
    if (!buf) return;
    free(buf->text);
    free(buf);
}

static void buffer_grow(GapBuffer *buf) {
    size_t new_capacity = buf->capacity * 2;
    size_t text_after_gap = buf->capacity - buf->gap_end;

    char *new_text = malloc(new_capacity);
    if (!new_text) return;

    /* Copy text before gap */
    memcpy(new_text, buf->text, buf->gap_start);

    /* Copy text after gap to new position */
    size_t new_gap_end = new_capacity - text_after_gap;
    memcpy(new_text + new_gap_end, buf->text + buf->gap_end, text_after_gap);

    free(buf->text);
    buf->text = new_text;
    buf->capacity = new_capacity;
    buf->gap_end = new_gap_end;
}

void buffer_insert(GapBuffer *buf, char c) {
    if (!buf) return;

    if (buf->gap_start == buf->gap_end) {
        buffer_grow(buf);
    }

    buf->text[buf->gap_start++] = c;
}

void buffer_delete(GapBuffer *buf) {
    if (!buf || buf->gap_start == 0) return;
    buf->gap_start--;
}

void buffer_move_gap(GapBuffer *buf, size_t pos) {
    if (!buf) return;

    if (pos < buf->gap_start) {
        /* Move gap left */
        size_t count = buf->gap_start - pos;
        memmove(buf->text + buf->gap_end - count,
                buf->text + pos, count);
        buf->gap_start = pos;
        buf->gap_end -= count;
    } else if (pos > buf->gap_start) {
        /* Move gap right */
        size_t count = pos - buf->gap_start;
        memmove(buf->text + buf->gap_start,
                buf->text + buf->gap_end, count);
        buf->gap_start = pos;
        buf->gap_end += count;
    }
}

char* buffer_get_text(GapBuffer *buf) {
    if (!buf) return NULL;

    size_t len = buffer_length(buf);
    char *text = malloc(len + 1);
    if (!text) return NULL;

    memcpy(text, buf->text, buf->gap_start);
    memcpy(text + buf->gap_start,
           buf->text + buf->gap_end,
           buf->capacity - buf->gap_end);
    text[len] = '\0';

    return text;
}

size_t buffer_length(GapBuffer *buf) {
    if (!buf) return 0;
    return buf->capacity - (buf->gap_end - buf->gap_start);
}

int buffer_load_file(GapBuffer *buf, const char *filename) {
    if (!buf || !filename) return -1;

    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    /* Reset buffer */
    buf->gap_start = 0;
    buf->gap_end = buf->capacity;

    int c;
    while ((c = fgetc(f)) != EOF) {
        buffer_insert(buf, (char)c);
    }

    fclose(f);
    return 0;
}

int buffer_save_file(GapBuffer *buf, const char *filename) {
    if (!buf || !filename) return -1;

    FILE *f = fopen(filename, "w");
    if (!f) return -1;

    /* Write text before gap */
    if (buf->gap_start > 0) {
        fwrite(buf->text, 1, buf->gap_start, f);
    }

    /* Write text after gap */
    size_t after = buf->capacity - buf->gap_end;
    if (after > 0) {
        fwrite(buf->text + buf->gap_end, 1, after, f);
    }

    fclose(f);
    return 0;
}
