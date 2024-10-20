#ifndef SCHEMA_H
#define SCHEMA_H


#include <stdio.h>
enum DB {
    CLIENT,
    CLIENT_AUTH,
    EMPLOYEE,
    EMPLOYEE_AUTH
};

typedef struct _employee {
    long long id;
    char name[50];
} employee;

typedef struct _client {
    long long id;
    char name[50];
    char account_number[15];
    char aadhaar[12];
} client;

typedef struct _employee_auth {
    char username[50];
    char password[50];
    long long emp_id;
} employee_auth;

typedef struct _client_auth {
    char username[50];
    char password[50];
    long long client_id;
} client_auth;

long long extract_id_from_record(enum DB db, void* record);

size_t get_record_size(enum DB db);

#endif