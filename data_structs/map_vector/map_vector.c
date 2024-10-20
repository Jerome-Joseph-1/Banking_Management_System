#include "map_vector.h"

map_vector* make_map_vector(size_t initial_size) {
    map_vector* v = (map_vector*)malloc(sizeof(map_vector));
    if (v == NULL) {
        fprintf(stderr, "Memory allocation failed for map_vector structure\n");
        return NULL;
    }
    
    v->maps = (void**)malloc(sizeof(void*) * initial_size);
    if (v->maps == NULL) {
        fprintf(stderr, "Memory allocation failed for map_vector maps\n");
        free(v);
        return NULL;
    }

    for (size_t i = 0; i < initial_size; i++) {
        v->maps[i] = NULL; // Initialize with NULL or create a specific map type if needed
    }

    v->size = initial_size;
    v->cur = -1;
    return v;
}

void* get_map(map_vector* mv, long long index) {
    if (index >= mv->size || index < 0) {
        return NULL;
    }
    return mv->maps[index];
}

void set_map_vector(map_vector* mv, size_t index, void* value) {
    if (index >= mv->size) {
        fprintf(stderr, "Index out of bounds in map_vector\n");
        return;
    }
    
    mv->maps[index] = value;
}
