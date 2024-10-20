#include "schema.h"

size_t get_record_size(enum DB db) {
    switch (db) {
        case CLIENT:
            return sizeof(client); 
        case CLIENT_AUTH:
            return sizeof(client_auth); 
        case EMPLOYEE:
            return sizeof(employee); 
        case EMPLOYEE_AUTH:
            return sizeof(employee_auth); 
        default:
            return 0; 
    }
}

long long extract_id_from_record(enum DB db, void* record) {
    switch (db) {
        case CLIENT: {
            client* rec = (client*)record;
            return rec->id;  // Extract client ID
        }
        case CLIENT_AUTH: {
            client_auth* rec = (client_auth*)record;
            return rec->client_id;  // Extract client_auth ID
        }
        case EMPLOYEE: {
            employee* rec = (employee*)record;
            return rec->id;  // Extract employee ID
        }
        case EMPLOYEE_AUTH: {
            employee_auth* rec = (employee_auth*)record;
            return rec->emp_id;  // Extract employee_auth ID
        }
        default:
            return -1;  // Return an invalid ID if an unknown enum is passed
    }
}