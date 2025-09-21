#include "assert.h"
#include "ctype.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define STRBUFFER_IMPLEMENTATION
#include "strbuffer.h"

/*
   1) read file and split on spaces/newlines
   2) count occurance of words in static hashtable
   3) count occurance of words in dynamic hastable
   4) make hashtable "generic"
*/

typedef size_t value_t;
typedef const char* key_t;

typedef struct {
    key_t key;
    value_t value;
    int occupied;
} HashCell;

typedef struct {
    size_t count;
    size_t capacity;
    HashCell* cells; // dynamic array
} HashTable;

int compare_cells(const void* a, const void* b) {
    const HashCell* acell = a;
    const HashCell* bcell = b;
    return (int)bcell->value - (int)acell->value;
}

void ht_sort(HashTable* ht) { qsort(ht->cells, ht->count, sizeof(ht->cells[0]), compare_cells); }

void ht_dump(HashTable ht, size_t topn) {
    if (topn <= 0) {
        topn = ht.capacity;
    }

    // for (size_t i = 0; i < ht.capacity && i < topn; i++) {
    size_t i = 0;
    while (topn > 0 && i < ht.capacity) {
        if (ht.cells[i].occupied == 1) {
            printf("Key: %s | Value: %zu | Occupied: %d\n", ht.cells[i].key, ht.cells[i].value, ht.cells[i].occupied);
            topn--;
        }
        i++;
    }
}

size_t compute_hash(key_t key) {
    size_t h      = 0;
    size_t length = strlen(key);
    for (size_t i = 0; i < length; i++) {
        h += (size_t)key[i];
    }
    return h;
}

void ht_init(HashTable* ht) {
    if (ht->capacity == 0) {
        ht->capacity = DEFAULT_CAPACITY;
    }

    // malloc
    ht->cells = malloc(ht->capacity * sizeof(*ht->cells));
    memset(ht->cells, 0, ht->capacity * sizeof(*ht->cells));
    ht->count = 0;
}

bool keycmp(key_t a, key_t b) {
    size_t la = strlen(a);
    size_t lb = strlen(b);
    if (la != lb) {
        return false;
    }
    return memcmp(a, b, la) == 0;
}

bool ht_set(HashTable* ht, key_t key, value_t value) {
    size_t hash = compute_hash(key) % ht->capacity;

    // Linear probing to find an empty slot or matching key
    for (size_t i = 0; i < ht->capacity; ++i) {
        if (!ht->cells[hash].occupied) {
            // Empty slot found, insert new cell
            ht->cells[hash].key      = strdup(key); // Store a copy of the key
            ht->cells[hash].value    = value;
            ht->cells[hash].occupied = 1;
            ht->count++;
            // printf("Inserting %s | %lu | %zu | %zu\n", key, strlen(key), hash, value);
            return true;
        } else if (keycmp((char*)ht->cells[hash].key, key)) {
            // Key already exists, update value
            ht->cells[hash].value    = value;
            ht->cells[hash].occupied = 1; // incase this was a deleted cell
            // printf("Inserting %s | %zu | %zu\n", key, hash, value);
            return true;
        }
        hash = (hash + 1) % ht->capacity;
    }
    // Table is full
    ht->capacity = ht->capacity * 2;
    ht->cells    = (HashCell*)realloc(ht->cells, sizeof(*ht->cells) * ht->capacity);
    assert(ht->cells != NULL && "HashTable Buffer Overflow");
    return false;
}

bool ht_get(HashTable* ht, key_t key, value_t* value) {
    size_t hash = compute_hash(key) % ht->capacity;

    // Linear probing
    for (size_t i = 0; i < ht->capacity; ++i) {
        if (ht->cells[hash].occupied != 1) {
            // Slot is empty, key not found
            return false;
        }
        if (keycmp((char*)ht->cells[hash].key, key)) {
            // Key found
            *value = ht->cells[hash].value;
            return true;
        }
        hash = (hash + 1) % ht->capacity;
    }
    // Key not found after probing all slots
    return false;
}

bool ht_remove(HashTable* ht, key_t key, value_t* result) {

    size_t hash = compute_hash(key) % ht->capacity;

    // Linear probing
    for (size_t i = 0; i < ht->capacity; ++i) {
        if (ht->cells[hash].occupied != 1) {
            // Slot is empty, key not found
            return false;
        }
        if (keycmp((char*)ht->cells[hash].key, key)) {
            // Key found
            *result                  = ht->cells[hash].value;
            ht->cells[hash].occupied = -1;
            return true;
        }
        hash = (hash + 1) % ht->capacity;
    }
    // Key not found after probing all slots
    return false;
}

void removeNonAlphanumeric(char* str) {
    int i, j;
    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (isalnum((unsigned char)str[i])) { // Check if character is alphanumeric
            str[j++] = str[i];                // Copy alphanumeric character to current position
        }
    }
    str[j] = '\0'; // Null-terminate the modified string
}

void convertToLower(char* str) {
    // Iterate through the string until the null terminator is reached
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

int main(int argc, char** argv) {

    char* filename = NULL;

    if (argc == 0) {
        printf("Hello World\n");
        printf("program args: %d\n", argc);
        return EXIT_SUCCESS;

    } else {
        filename = argv[1];
    }

    printf("Reading File: %s\n", filename);

    StringBuffer sb = {0};

    read_file_into_sb(filename, &sb);

    HashTable ht = {0};
    ht_init(&ht);
    size_t tmp = 0;

    while (sb.count > 0) {
        trim_left(&sb);
        char* token = chop_by_space(&sb);
        removeNonAlphanumeric(token);
        convertToLower(token);
        if (strlen(token) > 0) {
            if (ht_get(&ht, token, &tmp)) {
                tmp++;
                ht_set(&ht, token, tmp);
            } else {
                ht_set(&ht, token, 1);
            }
        }
    }
    ht_sort(&ht);
    value_t tmpv = 0;
    ht_remove(&ht, "the", &tmpv);

    ht_dump(ht, 6);
    return 0;
}
