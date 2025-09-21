#ifndef STRBUFFER_H_
#define STRBUFFER_H_

#include "assert.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>

#define DEFAULT_CAPACITY 256

typedef struct {
    char* items;
    size_t count;
    size_t capacity;
} StringBuffer;

static inline void print_sb(StringBuffer* sb) { printf("String Buffer: %s\nDONE\n", sb->items); }
static inline void free_sb(StringBuffer sb) { free(sb.items); }

void append(StringBuffer* sb, char c);
void extend(StringBuffer* sb, char* items);
void read_file_into_sb(char* filename, StringBuffer* sb);

#endif // STRBUFFER_H_

#define STRBUFFER_IMPLEMENTATION
#ifdef STRBUFFER_IMPLEMENTATION

void append(StringBuffer* sb, char c) {

    if (sb->count >= sb->capacity) {
        // increase capacity of items by 2
        size_t new_capacity;
        if (sb->capacity == 0) {
            new_capacity = DEFAULT_CAPACITY;
        } else {
            new_capacity = sb->capacity * 2;
        }

        sb->items = (char*)realloc(sb->items, sizeof(*sb->items) * new_capacity);
        assert(sb->items != NULL && "String Buffer Overflow");
    }
    // sb->count++;
    sb->items[sb->count++] = c;
}

void extend(StringBuffer* sb, char* items) {
    size_t items_length = strlen(items);

    if (sb->count + items_length > sb->capacity) {
        // alloc more mem

        if (sb->capacity == 0) {
            sb->capacity = DEFAULT_CAPACITY;
        }

        while (sb->count + items_length > sb->capacity) {
            sb->capacity = sb->capacity * 2;
        }

        sb->items = (char*)realloc(sb->items, sizeof(*sb->items) * sb->capacity);
        assert(sb->items != NULL && "String Buffer Overflow");
    }

    // memcpy into buffer
    char* dst = sb->items + sb->count; // mem location of last item in str
    memcpy(dst, items, items_length * sizeof(*sb->items));
    sb->count += items_length;
}

void read_file_into_sb(char* filename, StringBuffer* sb) {

    FILE* stream;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen(filename, "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1) {
        extend(sb, line);
    }

    free(line);
    fclose(stream);
}

char* chop_by_space(StringBuffer* sb) {
    size_t i = 0;
    while (i < sb->count && !isspace((unsigned char)sb->items[i])) {
        i += 1;
    }

    char* result = (char*)malloc(i + 1);
    if (!result) {
        perror("Failed to allocate memory in chop_by_space");
        exit(EXIT_FAILURE);
    }
    strncpy(result, sb->items, i);
    // result[i] = '\0';

    if (i < sb->count) {
        // skip the space
        // memmove(sb->items, sb->items + i + 1, sb->count - (i + 1));
        sb->count -= (i + 1);
		sb->items += (i + 1);
    } else {
        sb->count -= i;
		sb->items += i;
        // memmove(sb->items, sb->items + i, sb->count);
    }

    return result;
}

void trim_left(StringBuffer* sb) {
    size_t i = 0;
    while (i < sb->count && isspace(sb->items[i])) {
        i += 1;
    }

	sb->items = sb->items + i;
	sb->count = sb->count - i;
    // StringBuffer trimmed = {.items = sb->items + i, .count = sb->count - i, .capacity = sb->capacity};
    // return trimmed;
}

// typedef struct {
// 	char** words;
// 	size_t count;
// } StringArray;

// int split(StringBuffer *sb, StringArray * sa, char *delim) {
// 	char* token;
// 	sa->words = NULL;
// 	sa->count = 0;

// 	char* str_copy = strdup(sb->items);
// 	if (str_copy == NULL) {
//         perror("Failed to allocate memory for string copy");
//         return -1;
//     }
// 	token = strtok(str_copy, delim);
// 	while (token != NULL) {
// 		// get more mem
// 		// TODO do we need to do this each time?
// 		sa->words = (char**) realloc(sa->words, (sa->count + 1) * sizeof(char*));
// 		if (sa->words == NULL) {
//             perror("Failed to reallocate memory for sa->words array");
//             free(str_copy); // Free the copy before returning
//             // Free previously allocated sa->words if any
//             for (size_t i = 0; i < sa->count; i++) {
//                 free(sa->words[i]);
//             }
//             free(sa->words);
//             return -1;
//         }
// 		sa->words[sa->count] = strdup(token);
// 		if (sa->words[sa->count] == NULL) {
//             perror("Failed to allocate memory for a word");
//             free(str_copy);
//             // Free previously allocated sa->words
//             for (size_t i = 0; i < sa->count; i++) {
//                 free(sa->words[i]);
//             }
//             free(sa->words);
//             return -1;
//         }
// 		sa->count++;
// 		token = strtok(NULL, delim);
// 	}

// 	return 0;
// }

#endif // STRBUFFER_IMPLEMENTATION
