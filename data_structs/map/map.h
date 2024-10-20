#ifndef MAP_H
#define MAP_H

#include "../vector/vector.h"
#include "../pair/pair.h"

// custom pair vectors 

typedef struct _pvector {
    pair* arr;
    long long size;
    long long cur;
} p_vector;

bool push_back_pvector(p_vector* v, pair val);
p_vector* make_p_vector();

typedef struct _psvector {
    pair_str* arr;
    long long size;
    long long cur;
} ps_vector;

bool push_back_psvector(ps_vector* v, pair_str val);
ps_vector* make_ps_vector();

// custom map 

typedef struct _map_i {
    p_vector** arr;
    long long map_size;
} map_i;

typedef struct _map_s {
    ps_vector** arr;
    long long map_size;
} map_s;

long long hash_s(char* key);
long long hash_i(long long key);

bool insert_s(map_s* m, char* key, long long val);
bool insert_i(map_i* m, long long key, long long val);

long long find_s(map_s* m, char* key, bool* found); 
long long find_i(map_i* m, long long key, bool* found); 

size_t map_size_s(map_s*); 
size_t map_size_i(map_i*); 

map_s* make_map_s(); 
map_i* make_map_i(); 

void free_map_s(map_s*);
void free_map_i(map_i*);

#endif
