#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h> 

typedef struct _vector {
    long long* arr;
    long long size;
    long long cur;
} vector;

vector* make_vector(size_t initial_size); 
void free_vector(vector* v); 

bool push_back(vector* v, long long val); 
long long pop_back(vector* v); 

size_t vector_size(const vector* v); 
bool is_empty(const vector* v); 

long long vector_get(vector* v, size_t index);

#endif