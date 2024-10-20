#include "map.h"
#include "../pair/pair.h"
#include <stdlib.h>
#include <string.h>

long long MAP_SIZE_MAX = 10000;

long long hash_s(char* key) {
    long long hash = 5381; 
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  
    }

    return (hash % MAP_SIZE_MAX + MAP_SIZE_MAX) % MAP_SIZE_MAX;
}


long long hash_i(long long key) {
    
    key = ((key >> 30) ^ key) * 0xBF58476D1CE4E5B9; 
    key = ((key >> 27) ^ key) * 0x94D049BB133111EB; 
    key = key ^ (key >> 31); 

    return key % MAP_SIZE_MAX;  
}

p_vector* make_p_vector() {
    p_vector* v = (p_vector*)malloc(sizeof(p_vector));
    if (v == NULL) {
        fprintf(stderr, "Memory allocation failed for vector structure\n");
        return NULL;
    }
    
    v->arr = (pair*)malloc(sizeof(pair));
    if (v->arr == NULL) {
        fprintf(stderr, "Memory allocation failed for vector array\n");
        free(v);
        return NULL;
    }
    
    v->size = 1;
    v->cur = -1;
    return v;
}

ps_vector* make_ps_vector() {
    ps_vector* v = (ps_vector*)malloc(sizeof(ps_vector));
    if(v == NULL) {
        fprintf(stderr, "Memory allocation failed for vector structure\n");
        return NULL;
    }

    v->arr = (pair_str*)malloc(sizeof(pair_str));
    if (v->arr == NULL) {
        fprintf(stderr, "Memory allocation failed for vector array\n");
        free(v);
        return NULL;
    }
    
    v->size = 1;
    v->cur = -1;
    return v;
}

bool push_back_pvector(p_vector* v, pair val) {
    if (v == NULL) return false;

    if (v->cur >= v->size - 1) {
        v->size <<= 1; 
        pair* temp = (pair*)realloc(v->arr, sizeof(pair) * v->size);
        if (temp == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }
        v->arr = temp;
    }
    v->arr[++v->cur] = val;
    return true;
}

bool push_back_psvector(ps_vector* v, pair_str val) {
    if (v == NULL) return false;

    if (v->cur >= v->size - 1) {
        v->size <<= 1; 
        pair_str* temp = (pair_str*)realloc(v->arr, sizeof(pair_str) * v->size);
        if (temp == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }
        v->arr = temp;
    }
    v->arr[++v->cur] = val;
    return true;
}

map_i* make_map_i() {
    map_i* m = (map_i*)malloc(sizeof(map_i));

    if(m == NULL) {
        fprintf(stderr, "Memory allocation failed for map structure(1) \n");
        return NULL;
    }

    m->map_size = 0;

    m->arr = (p_vector**)malloc(sizeof(p_vector*) * MAP_SIZE_MAX);

    if(m->arr == NULL) {
        fprintf(stderr, "Memory allocation failed for map structure(2) \n");
        free(m);
        return NULL;
    }

    for (size_t i = 0; i < MAP_SIZE_MAX; ++i) {
        m->arr[i] = make_p_vector();

        if(m->arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for map structure(2) \n");
            return NULL;
        }
    }

    return m;
}

map_s* make_map_s() {
    map_s* m = (map_s*)malloc(sizeof(map_s));

    if(m == NULL) {
        fprintf(stderr, "Memory allocation failed for map structure(1) \n");
        return NULL;
    }

    m->map_size = 0;

    m->arr = (ps_vector**)malloc(sizeof(ps_vector*) * MAP_SIZE_MAX);

    if(m->arr == NULL) {
        fprintf(stderr, "Memory allocation failed for map structure(2) \n");
        free(m);
        return NULL;
    }

    for (size_t i = 0; i < MAP_SIZE_MAX; ++i) {
        m->arr[i] = make_ps_vector();

        if(m->arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for map structure(2) \n");
            return NULL;
        }
    }

    return m;
}

bool insert_s(map_s* m, char* key, long long val) {
    long long hashed_key = hash_s(key);

    ps_vector* arr = m->arr[hashed_key];
    pair_str* p = make_pair_str(key, val);

    if(p == NULL) {
        fprintf(stderr, "Memory allocation failed for pair creation -> for map in insert_i\n");
        return false;
    }

    for (long long i = 0; i < arr->size; ++i) {
        if(strcmp(arr->arr[i].first, key) == 0) {
            m->map_size++;
            arr->arr[i].second = val;
            return true;
        }
    }

    bool res = push_back_psvector(arr, *p);

    if(res == false) {
        fprintf(stderr, "Failed to push value in insert_i\n");
        return false;
    }

    m->map_size++;
    return true;
}

bool insert_i(map_i* m, long long key, long long val) {
    long long hashed_key = hash_i(key);

    p_vector* arr = m->arr[hashed_key];
    pair* p = make_pair(key, val);

    if(p == NULL) {
        fprintf(stderr, "Memory allocation failed for pair creation -> for map in insert_i\n");
        return false;
    }

    for (long long i = 0; i < arr->size; ++i) {
        if(arr->arr[i].first == key) {
            arr->arr[i].second = val;
            m->map_size++;
            return true;
        }
    }

    bool res = push_back_pvector(arr, *p);

    if(res == false) {
        fprintf(stderr, "Failed to push value in insert_i\n");
        return false;
    }
    m->map_size++;

    return true;
}

long long find_i(map_i* m, long long key, bool* found) {
    long long hashed_key = hash_i(key);
    
    p_vector* arr = m->arr[hashed_key];

    long long map_size = arr->size;

    for (size_t i = 0; i < map_size; ++i) {
        if(arr->arr[i].first == key) {
            *found = true;
            return arr->arr[i].second;
        }
    }

    *found = false;
    return 0;
}

long long find_s(map_s* m, char* key, bool* found) {
    long long hashed_key = hash_s(key);

    ps_vector* arr = m->arr[hashed_key];

    long long map_size = arr->size;

    for (size_t i = 0; i < map_size; ++i) {
        if(strcmp(arr->arr[i].first, key) == 0) {
            *found = true;
            return arr->arr[i].second;
        }
    }

    *found = false;
    return 0;    
}

// void print_i(map_i* m) {
//     for (long long hashed_key = 0; hashed_key < 4; ++hashed_key) {
//         p_vector* arr = m->arr[hashed_key];
//         printf("Size: %lld\n", arr->size);
//         for (size_t i = 0; i < 4; ++i) {
//             printf("<%lld, %lld>\n", arr->arr[i].first, arr->arr[i].second);
//         }
//     }
// }

size_t map_size_s(map_s* m) {
    return 0;
} 
size_t map_size_i(map_i*); 