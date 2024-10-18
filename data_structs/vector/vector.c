#include "vector.h"
#include <stdio.h> 

vector* make_vector(size_t initial_size) {
    vector* v = (vector*)malloc(sizeof(vector));
    if (v == NULL) {
        fprintf(stderr, "Memory allocation failed for vector structure\n");
        return NULL;
    }
    
    v->arr = (long long*)malloc(sizeof(long long) * initial_size);
    if (v->arr == NULL) {
        fprintf(stderr, "Memory allocation failed for vector array\n");
        free(v);
        return NULL;
    }
    
    v->size = initial_size;
    v->cur = -1;
    return v;
}

void free_vector(vector* v) {
    if (v != NULL) {
        free(v->arr);
        free(v);
    }
}

bool push_back(vector* v, long long val) {
    if (v == NULL) return false;

    if (v->cur >= v->size - 1) {
        v->size <<= 1; 
        long long* temp = (long long*)realloc(v->arr, sizeof(long long) * v->size);
        if (temp == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }
        v->arr = temp;
    }
    v->arr[++v->cur] = val;
    return true;
}

long long pop_back(vector* v) {
    if (v == NULL || v->cur < 0) {
        return 0;
    }

    return v->arr[v->cur--];
}

size_t vector_size(const vector* v) {
    if (v == NULL) return 0;
    return v->cur + 1;
}

bool is_empty(const vector* v) {
    return (v == NULL || v->cur < 0);
}

long long vector_get(vector* v, size_t index) {
    if(index < 0 || v == NULL || index > v->cur)
        return 0;
    return v->arr[index];
}