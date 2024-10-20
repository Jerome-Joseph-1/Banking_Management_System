#include "map_vector.h"

map_vector_i* make_map_vector_i(size_t initial_size) {
    map_vector_i* v = (map_vector_i*)malloc(sizeof(map_vector_i));
    if (v == NULL) {
        fprintf(stderr, "Memory allocation failed for map_vector_i structure\n");
        return NULL;
    }
    
    v->maps = (map_i**)malloc(sizeof(map_i*) * initial_size);
    if (v->maps == NULL) {
        fprintf(stderr, "Memory allocation failed for map_vector_i mapsay\n");
        free(v);
        return NULL;
    }

    for(size_t i = 0; i < initial_size; i++)
        v->maps[i] = make_map_i();

    v->size = initial_size;
    v->cur = -1;
    return v;
}

map_i* get_map(map_vector_i* mv, long long index) {
    if(index >= mv->size || index < 0)
        return NULL;
    return mv->maps[index];
}

void set_map_vector_i(map_vector_i* mv, size_t index, map_i* value) {
    if (index >= mv->size) {
        fprintf(stderr, "Index out of bounds in map_vector_i\n");
        return;
    }
    
    mv->maps[index] = value;
}


// TODO: 
// bool push_vector_map(map_vector_i* mv, map_i* m) {
//     if (mv == NULL) return false;

//     if (mv->cur >= mv->size - 1) {
//         mv->size <<= 1; 
//         long long* temp = (long long*)realloc(mv->maps, sizeof(long long) * v->size);
//         if (temp == NULL) {
//             fprintf(stderr, "Memory reallocation failed\n");
//             return false;
//         }
//         v->arr = temp;
//     }
//     v->arr[++v->cur] = val;
//     return true;
// }