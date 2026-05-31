#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Each bucket now stores the key and the value to handle collisions safely
typedef struct Bucket {
    char *key;
    void *val;
} Bucket;

typedef struct Map {
    size_t size;
    size_t cap;
    Bucket *buckets; 
} Map;

// Standard FNV-1a constants
const size_t BASE = 0x811c9dc5;
const size_t PRIME = 0x01000193;

// Core FNV-1a hashing algorithm
size_t hash(Map *m, char *str) {
    size_t h = BASE;
    while (*str) {
        h ^= *str++;
        h *= PRIME;
    }
    return h & (m->cap - 1);
}

// Allocates and initializes the map using calloc to ensure all entries start empty (NULL)
Map init(size_t cap) {
    Map m = {0, cap, NULL};
    
    m.buckets = calloc(cap, sizeof(Bucket));
    assert(m.buckets != NULL);
    
    return m;
}

// Inserts or updates a key-value pair using linear probing for collisions
void put(Map *m, char *key, void *val) {
    size_t idx = hash(m, key);
    
    while (m->buckets[idx].key != NULL) {
        if (strcmp(m->buckets[idx].key, key) == 0) {
            m->buckets[idx].val = val;
            return;
        }
        idx = (idx + 1) & (m->cap - 1); 
    }
    
    m->buckets[idx].key = key;
    m->buckets[idx].val = val;
    m->size++;
}

// Retrieves a value by its key using linear probing
void *get(Map *m, char *key) {
    size_t idx = hash(m, key);
    
    while (m->buckets[idx].key != NULL) {
        if (strcmp(m->buckets[idx].key, key) == 0) {
            return m->buckets[idx].val; 
        }
        idx = (idx + 1) & (m->cap - 1);
    }
    
    return NULL; 
}

int main(void) {
    Map m = init(1024);
    double d1 = 25.0;
    double d2 = 50.0;
    
    put(&m, "key1", (void*)&d1);
    put(&m, "key2", (void*)&d2);

    // This will cleanly print: key1=25.000000;key2=50.000000
    printf("key1=%f;key2=%f\n", *(double*)get(&m, "key1"), *(double*)get(&m, "key2"));

    free(m.buckets);
    return EXIT_SUCCESS;
}
