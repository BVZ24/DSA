#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// Tombstone pointer to mark deleted slots without breaking linear probing chains
static char *const TOMBSTONE = (char *)(uintptr_t)0x1;

typedef struct Bucket {
    char *key;
    void *val;
} Bucket;

typedef struct Map {
    size_t size;
    size_t cap;
    size_t tombstone_count; // FIX #4: track tombstones to prevent infinite probe loops
    Bucket *buckets; 
} Map;

// Standard FNV-1a constants
const size_t BASE = 0x811c9dc5;
const size_t PRIME = 0x01000193;

// Core FNV-1a hashing algorithm
static size_t hash(Map *m, const char *str) {
    size_t h = BASE;
    while (*str) {
        h ^= (unsigned char)*str++;
        h *= PRIME;
    }
    return h & (m->cap - 1);
}

// Allocates and initializes the map
Map map_init(size_t initial_cap) {
    // Ensure capacity is a power of two (minimum 16 for production stability)
    size_t cap = 16;
    while (cap < initial_cap) {
        cap <<= 1;
    }

    Map m = {0, cap, 0, NULL};
    m.buckets = calloc(cap, sizeof(Bucket));
    assert(m.buckets != NULL);
    return m;
}

// Destroys the map and frees all internally allocated memory
void map_free(Map *m) {
    if (!m || !m->buckets) return;
    for (size_t i = 0; i < m->cap; i++) {
        if (m->buckets[i].key != NULL && m->buckets[i].key != TOMBSTONE) {
            free(m->buckets[i].key); // Free our internal strdup copy
        }
    }
    free(m->buckets);
    m->buckets = NULL;
    m->size = 0;
    m->cap = 0;
    m->tombstone_count = 0;
}

// Internal function to resize the table when it gets 75% full
// Also used to compact tombstones (called with same cap)
static void map_resize(Map *m, size_t new_cap) {
    Bucket *old_buckets = m->buckets;
    size_t old_cap = m->cap;

    m->buckets = calloc(new_cap, sizeof(Bucket));
    assert(m->buckets != NULL);
    m->cap = new_cap;
    m->size = 0;           // Reset size; put() will increment it accurately
    m->tombstone_count = 0; // FIX #4: tombstones are eliminated on rehash

    // Re-hash all valid elements into the new array
    for (size_t i = 0; i < old_cap; i++) {
        if (old_buckets[i].key != NULL && old_buckets[i].key != TOMBSTONE) {
            // Put directly back into the new array
            size_t idx = hash(m, old_buckets[i].key);
            while (m->buckets[idx].key != NULL) {
                idx = (idx + 1) & (m->cap - 1);
            }
            m->buckets[idx].key = old_buckets[i].key;
            m->buckets[idx].val = old_buckets[i].val;
            m->size++;
        }
    }
    free(old_buckets);
}

// Inserts or updates a key-value pair dynamically
void map_put(Map *m, const char *key, void *val) {
    // FIX #4: grow if live entries exceed 75%, OR if live+tombstone occupancy
    // exceeds 87.5% (7/8) — this prevents a tombstone-filled table with no
    // NULL slots, which would cause infinite probe loops in get/del.
    if (m->size * 4 >= m->cap * 3) {
        map_resize(m, m->cap * 2);
    } else if ((m->size + m->tombstone_count) * 8 >= m->cap * 7) {
        map_resize(m, m->cap); // Rehash in-place to purge tombstones
    }

    size_t idx = hash(m, key);
    size_t first_tombstone_idx = (size_t)-1;

    while (m->buckets[idx].key != NULL) {
        if (m->buckets[idx].key == TOMBSTONE) {
            if (first_tombstone_idx == (size_t)-1) {
                first_tombstone_idx = idx; // Keep track of the first recycling spot
            }
        } else if (strcmp(m->buckets[idx].key, key) == 0) {
            // Key matches, overwrite the value
            m->buckets[idx].val = val;
            return;
        }
        idx = (idx + 1) & (m->cap - 1);
    }

    // If we encountered a tombstone during probing, recycle it to save space
    if (first_tombstone_idx != (size_t)-1) {
        idx = first_tombstone_idx;
        m->tombstone_count--; // FIX #4: slot is being reclaimed, not left as tombstone
    }

    m->buckets[idx].key = strdup(key); // Secure ownership of the string
    assert(m->buckets[idx].key != NULL);
    m->buckets[idx].val = val;
    m->size++;
}

// Retrieves a value by its key
void *map_get(Map *m, const char *key) {
    size_t idx = hash(m, key);
    size_t probed = 0; // FIX #4: bound the probe loop to cap iterations

    while (m->buckets[idx].key != NULL && probed < m->cap) {
        if (m->buckets[idx].key != TOMBSTONE && strcmp(m->buckets[idx].key, key) == 0) {
            return m->buckets[idx].val; // Found!
        }
        idx = (idx + 1) & (m->cap - 1);
        probed++;
    }
    return NULL; // Not found
}

// Deletes an entry safely using a tombstone flag
bool map_del(Map *m, const char *key) {
    size_t idx = hash(m, key);
    size_t probed = 0; // FIX #4: bound the probe loop to cap iterations

    while (m->buckets[idx].key != NULL && probed < m->cap) {
        if (m->buckets[idx].key != TOMBSTONE && strcmp(m->buckets[idx].key, key) == 0) {
            free(m->buckets[idx].key);
            m->buckets[idx].key = TOMBSTONE; // Plant a tombstone
            m->buckets[idx].val = NULL;
            m->size--;
            m->tombstone_count++; // FIX #4: track the new tombstone
            return true; 
        }
        idx = (idx + 1) & (m->cap - 1);
        probed++;
    }
    return false; // Key didn't exist
}

// --- Production Test Main Execution ---
int main(void) {
    // Initializing with a tiny capacity of 16 to actively force dynamic resizing
    Map m = map_init(16);

    double d1 = 25.0;
    double d2 = 50.0;
    double d3 = 75.0;
    double d4 = 100.0;
    
    // 1. Basic inserts
    map_put(&m, "key1", &d1);
    map_put(&m, "key2", &d2);
    printf("Initial fetch: key1=%f, key2=%f\n", *(double*)map_get(&m, "key1"), *(double*)map_get(&m, "key2"));

    // 2. Safe Deletion and Verification
    map_del(&m, "key1");
    if (map_get(&m, "key1") == NULL) {
        printf("Safe Deletion verified: 'key1' is cleanly gone.\n");
    }
    
    // Probing chain integrity test ("key2" can still be found perfectly past key1's tombstone)
    printf("Probing Chain intact: key2=%f\n", *(double*)map_get(&m, "key2"));

    // 3. Dynamic Resize Test
    // Pushing elements to force the map to grow dynamically way past its initial size of 16
    char key_buffer[32]; // FIX #3: enlarged buffer to safely hold any key_N string
    for (int i = 0; i < 100; i++) {
        snprintf(key_buffer, sizeof(key_buffer), "bulk_key_%d", i); // FIX #3: snprintf instead of sprintf
        map_put(&m, key_buffer, &d3);
    }
    
    map_put(&m, "final_check", &d4);
    printf("Post-Resize Growth check: map capacity expanded seamlessly to %zu items.\n", m.cap);
    printf("Fetched post-resize value: %f\n", *(double*)map_get(&m, "final_check"));

    // Clean up all resources
    map_free(&m);
    return EXIT_SUCCESS;
}
