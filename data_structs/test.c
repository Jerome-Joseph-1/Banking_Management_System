#include "map/map.h"
#include <stdio.h>

int main() {
    map_s* m = make_map_s();

    insert_s(m, "apple", 10);
    insert_s(m, "ban", 20);
    insert_s(m, "ooooaaa", 20);
    insert_s(m, "pppwwww", 541651);
    insert_s(m, "ban", 321);
    insert_s(m, "Ban", 7848);
    insert_s(m, "apple", 78485);

    char arr[][8] = {
        "apple",
        "ban",
        "ooooaaa",
        "pppwwww",
        "Ban"
    };
    
    bool x = false;
    for (int i = 0; i < 5; ++i) {
        printf("%s : %lld\n", arr[i], find_s(m, arr[i], &x));
    }
    return 0;
}