#include "vector.h"
#include <stdio.h>

int main() {
    vector* v = make_vector(5);

    push_back(v, 7);
    push_back(v, 8);
    push_back(v, 3);
    push_back(v, 5);
    push_back(v, 2);

    for (size_t i = 0; i < vector_size(v); ++i) {
        printf("%lld ", vector_get(v, i));
    }

    printf("\n");

    return 0;
}