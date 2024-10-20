#ifndef MAP_VECTOR_H
#define MAP_VECTOR_H

#include "../map/map.h"

typedef struct _map_vector_i {
    map_i** maps;
    long long size;
    long long cur;
} map_vector_i;

map_vector_i* make_map_vector_i(size_t initial_size);

map_i* get_map(map_vector_i* mv, long long index);

void set_map_vector_i(map_vector_i* mv, size_t index, map_i* value);

// TODO:
// bool push_vector_map(map_vector_i* mv, map_i* m);

#endif