#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct {
    char *text;
    size_t gap_start;
    size_t gap_end;
    size_t capacity;
} GapBuffer;

/* Initialize a new gap buffer */
GapBuffer* buffer_new(void);

/* Free gap buffer memory */
void buffer_free(GapBuffer *buf);

/* Insert character at current gap position */
void buffer_insert(GapBuffer *buf, char c);

/* Delete character before gap position */
void buffer_delete(GapBuffer *buf);

/* Move gap to specified position */
void buffer_move_gap(GapBuffer *buf, size_t pos);

/* Get buffer content as string (caller must free) */
char* buffer_get_text(GapBuffer *buf);

/* Get buffer length (excluding gap) */
size_t buffer_length(GapBuffer *buf);

/* Load file into buffer */
int buffer_load_file(GapBuffer *buf, const char *filename);

/* Save buffer to file */
int buffer_save_file(GapBuffer *buf, const char *filename);

#endif
