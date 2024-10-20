#include "schema.h"
#include <stdlib.h>
#include <string.h>

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

char* extract_aadhaar_from_record(enum DB db, void* record) {
    switch (db) {
        case CLIENT: {
            client* rec = (client*)record;
            char* aadhaar = malloc(13);  // Allocate memory for Aadhaar (12 digits + null terminator)
            if (aadhaar) {
                strncpy(aadhaar, rec->aadhaar, 12);
                aadhaar[12] = '\0';  // Null terminate the string
            }
            return aadhaar;  // Return the extracted Aadhaar number
        }
        default:
            return NULL;  // Return NULL if Aadhaar extraction is not applicable
    }
}

char* extract_account_number_from_record(enum DB db, void* record) {
    switch (db) {
        case CLIENT: {
            client* rec = (client*)record;
            char* account_number = malloc(16);  // Allocate memory for account number (15 digits + null terminator)
            if (account_number) {
                strncpy(account_number, rec->account_number, 15);
                account_number[15] = '\0';  // Null terminate the string
            }
            return account_number;  // Return the extracted account number
        }
        default:
            return NULL;  // Return NULL if account number extraction is not applicable
    }
}
