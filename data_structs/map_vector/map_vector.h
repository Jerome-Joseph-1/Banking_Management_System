#ifndef MAP_VECTOR_H
#define MAP_VECTOR_H

#include "../map/map.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct _map_vector {
    void** maps;
    long long size;
    long long cur;
} map_vector;

map_vector* make_map_vector(size_t initial_size);
void* get_map(map_vector* mv, long long index);
void set_map_vector(map_vector* mv, size_t index, void* value);

#endif
