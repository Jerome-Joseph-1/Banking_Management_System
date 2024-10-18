#ifndef PAIR_H
#define PAIR_H

#include <stdlib.h>
#include <stdio.h>

typedef struct _pair {
    long long first;
    long long second;
} pair;

typedef struct _pair_str {
    char first[20]; // TODO: Make char vector 
    long long second;
} pair_str;

pair* make_pair(long long first, long long second);

pair_str* make_pair_str(char* first, long long second);

#endif