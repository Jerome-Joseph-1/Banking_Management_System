#include "map/map.h"
#include <stdio.h>

int main() {
    map_i* m = make_map_i();

    insert_i(m, 5, 10);
    insert_i(m, 5, 20);
    insert_i(m, 15, 20);
    insert_i(m, 25, 541651);
    insert_i(m, 5, 321);
    insert_i(m, 12, 7848);
    insert_i(m, 15, 78485);
    

    long long arr[] = {15, 5, 25, 12, 15};
    
    bool x = false;
    for (int i = 0; i < 5; ++i) {
        printf("%lld : %lld\n", arr[i], find_i(m, arr[i], &x));
    }
    return 0;
}