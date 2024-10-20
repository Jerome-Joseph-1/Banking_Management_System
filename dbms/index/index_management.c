#include "index_management.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void initialize_indexes(map_vector* indexes) {
    for (int i = 0; i < indexes->size; i++) {
        enum DB db = (enum DB)i;
        if (db == EMPLOYEE) {
            employee_indexes* emp_indexes = malloc(sizeof(employee_indexes));
            emp_indexes->id_index = make_map_i();
            indexes->maps[i] = emp_indexes;
        } else if (db == CLIENT) {
            client_indexes* cli_indexes = malloc(sizeof(client_indexes));
            cli_indexes->id_index = make_map_i();
            cli_indexes->aadhaar_index = make_map_s();
            cli_indexes->account_number_index = make_map_s();
            indexes->maps[i] = cli_indexes;
        }
    }
}

void rebuild_indexes(enum DB db, map_vector* indexes) {
    char db_filename[256];
    snprintf(db_filename, sizeof(db_filename), "db/%d.db", db);

    int fd = open(db_filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open database file for indexing");
        return;
    }

    if (db == EMPLOYEE) {
        employee_indexes* emp_indexes = (employee_indexes*)indexes->maps[db];
        void* record = malloc(get_record_size(db));
        long long position = 0;
        
        while (read(fd, record, get_record_size(db)) > 0) {
            long long id = extract_id_from_record(db, record);
            insert_i(emp_indexes->id_index, id, position);
            position += get_record_size(db);
        }
        free(record);
    } else if (db == CLIENT) {
        client_indexes* cli_indexes = (client_indexes*)indexes->maps[db];
        void* record = malloc(get_record_size(db));
        long long position = 0;

        while (read(fd, record, get_record_size(db)) > 0) {
            long long id = extract_id_from_record(db, record);
            char* aadhaar = extract_aadhaar_from_record(db, record);
            char* account_number = extract_account_number_from_record(db, record);

            insert_i(cli_indexes->id_index, id, position);
            insert_s(cli_indexes->aadhaar_index, aadhaar, position);
            insert_s(cli_indexes->account_number_index, account_number, position);

            position += get_record_size(db);
        }
        free(record);
    }
    close(fd);
}

void update_indexes(enum DB db, void* data_struct, size_t size, map_vector* indexes) {
    if (db == EMPLOYEE) {
        employee_indexes* emp_indexes = (employee_indexes*)indexes->maps[db];
        long long id = extract_id_from_record(db, data_struct);
        bool found = false;
        long long start_position = find_i(emp_indexes->id_index, id, &found);

        if (!found) {
            insert_i(emp_indexes->id_index, id, start_position);
        }
    } else if (db == CLIENT) {
        client_indexes* cli_indexes = (client_indexes*)indexes->maps[db];
        long long id = extract_id_from_record(db, data_struct);
        char* aadhaar = extract_aadhaar_from_record(db, data_struct);
        char* account_number = extract_account_number_from_record(db, data_struct);

        bool found = false;
        long long start_position = find_i(cli_indexes->id_index, id, &found);

        if (!found) {
            insert_i(cli_indexes->id_index, id, start_position);
        }

        insert_s(cli_indexes->aadhaar_index, aadhaar, start_position);
        insert_s(cli_indexes->account_number_index, account_number, start_position);
    }
}
