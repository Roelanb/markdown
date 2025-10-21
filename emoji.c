#include <string.h>
#include "emoji.h"

const char* emoji_find(const char *shortcode) {
    if (!shortcode) return NULL;

    for (int i = 0; emoji_map[i].shortcode != NULL; i++) {
        if (strcmp(emoji_map[i].shortcode, shortcode) == 0) {
            return emoji_map[i].unicode;
        }
    }

    return NULL;
}
