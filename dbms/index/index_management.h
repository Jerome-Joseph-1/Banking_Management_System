#ifndef INDEX_MANAGEMENT_H
#define INDEX_MANAGEMENT_H

#include "../schema/schema.h"
#include "../../data_structs/map_vector/map_vector.h"

typedef struct _employee_indexes {
    map_i* id_index; // Index for employee ID
} employee_indexes;

typedef struct _client_indexes {
    map_i* id_index;            // Index for client ID
    map_s* aadhaar_index;       // Index for Aadhaar number
    map_s* account_number_index; // Index for account number
} client_indexes;


void initialize_indexes(map_vector* indexes);
void rebuild_indexes(enum DB db, map_vector* indexes);
void update_indexes(enum DB db, void* data_struct, size_t size, map_vector* indexes);

#endif
