#include "map/map.h"
#include "map_vector/map_vector.h"
#include <stdio.h>

int main() {
    map_vector_i* mv = make_map_vector_i(2);

    map_i* m = get_map(mv, 0);

    insert_i(m, 1, 10);
    insert_i(m, 2, 20);
    insert_i(m, 1, 20);
    insert_i(m, 7, 541651);
    insert_i(m, 65, 321);
    insert_i(m, 65, 7848);
    insert_i(m, 132, 78485);

    long long arr[] = {
        1,
        2,
        7,
        65,
        132
    };
    
    bool x = false;
    for (int i = 0; i < 5; ++i) {
        printf("%lld : %lld\n", arr[i], find_i(m, arr[i], &x));
    }
    return 0;
}