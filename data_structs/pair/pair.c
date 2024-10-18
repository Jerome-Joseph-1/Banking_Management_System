#include "pair.h"
#include <string.h>

pair* make_pair(long long first, long long second) {
    pair* p = (pair*)malloc(sizeof(pair));

    if(p == NULL) {
        fprintf(stderr, "Memory allocation failed for pair structure \n");
        return NULL;
    }

    p->first = first;
    p->second = second;

    return p;
}
pair_str* make_pair_str(char* first, long long second) {
    pair_str* p = (pair_str*)malloc(sizeof(pair_str));

    if(p == NULL) {
        fprintf(stderr, "Memory allocation failed for pair_str structure \n");
        return NULL;
    }

    strcpy(p->first, first);
    p->second = second;

    return p;
}

void free_pair_i(pair* p) {
    free(p);
}

void free_pair_s(pair_str* p) {
    free(p);
}