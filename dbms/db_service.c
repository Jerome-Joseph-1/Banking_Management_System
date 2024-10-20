#include <sys/stat.h>
#include "load_store/load_store.h"
#include "schema/schema.h"
#include "../data_structs/map_vector/map_vector.h"
#include "../data_structs/vector/vector.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

map_vector_i* indexes;
vector* db_fds;  // Vector to store file descriptors for each DB

void set_map(map_vector_i* indexes, enum DB db, map_i* db_index) {
    if (db < 0 || db >= indexes->size) {
        fprintf(stderr, "Invalid table index for DB enum: %d\n", db);
        return;
    }

    // Set the map at the corresponding index (db) in the vector
    set_map_vector_i(indexes, db, db_index);
}

void initialize_db() {
    // Determine the number of tables and allocate space for indexes and file descriptors
    size_t table_count = 4;  // Example: 4 tables (CLIENT, CLIENT_AUTH, EMPLOYEE, EMPLOYEE_AUTH)
    indexes = make_map_vector_i(table_count);
    db_fds = make_vector(table_count);

    // Open database files and load the indexes
    for (int i = 0; i < table_count; i++) {
        enum DB db = (enum DB)i;

        // Construct filenames for DB files and index files
        char db_filename[256], index_filename[256];
        snprintf(db_filename, sizeof(db_filename), "db/%d.db", db);
        snprintf(index_filename, sizeof(index_filename), "index/%d.index", db);

        // Open the database file and store the file descriptor in the vector
        int fd = open(db_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("Failed to open database file");
            exit(EXIT_FAILURE);
        }
        push_back(db_fds, fd);

        // Load the index from the index file
        map_i* db_index = make_map_i();  // Create an empty index map

        int index_fd = open(index_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (index_fd == -1) {
            perror("Failed to open index file");
            exit(EXIT_FAILURE);
        }

        // Read index from the file if it exists, otherwise initialize an empty index
        struct stat st;
        if (fstat(index_fd, &st) == 0 && st.st_size > 0) {
            // File exists and has data, load it
            long long id, position;
            while (read(index_fd, &id, sizeof(long long)) > 0) {
                read(index_fd, &position, sizeof(long long));
                insert_i(db_index, id, position);  // Insert into the map
            }
        } else {
            printf("Initializing empty index for table %d\n", db);
        }

        close(index_fd);

        set_map(indexes, db, db_index);  // Store the index map in the vector
    }
}


int get_fd_of_db(enum DB db) {
    // Retrieve the file descriptor for the given DB from the vector
    return vector_get(db_fds, db);
}

void insert_to_db(enum DB db, void* data_struct, size_t size) {
    // NOTE: Ensure 'db' is a valid enum value
    int fd = get_fd_of_db(db);
    map_i* db_index = get_map(indexes, db);

    long long id = extract_id_from_record(db, data_struct);

    bool found = false;
    long long start_position = find_i(db_index, id, &found);

    if (!found) {
        // If not found, append at the end (lseek(fd, 0, SEEK_END) to get the file size)
        start_position = lseek(fd, 0, SEEK_END);
        insert_i(db_index, id, start_position);
    }

    // Store the record in the DB file
    store_bytes_to_file(fd, start_position, data_struct, size);
}

// SOCKET VERSION
// void fetch_from_db(enum DB db, long long id, size_t size) {
//     int fd = get_fd_of_db(db);
//     map_i* db_index = get_map(indexes, db);

//     bool found = false;
//     long long start_position = find_i(db_index, id, &found);

//     if (!found) {
//         printf("Record not found\n");
//         return;
//     }

//     void* data = malloc(size);
//     load_bytes_to_file(fd, start_position, data, size);

//     // Send data over the socket to the corresponding request (not fully implemented here)
//     // send(socket_fd, data, size, 0);

//     free(data);
// }

void fetch_from_db(enum DB db, long long id, size_t size) {
    int fd = get_fd_of_db(db);
    map_i* db_index = get_map(indexes, db);

    bool found = false;
    long long start_position = find_i(db_index, id, &found);

    if (!found) {
        printf("Record not found for ID %lld\n", id);
        return;
    }

    void* data = malloc(size);
    load_bytes_to_file(fd, start_position, data, size);

    // Print the data based on the table (db) type
    switch (db) {
        case EMPLOYEE: {
            employee* emp = (employee*)data;
            printf("Employee ID: %lld, Name: %s\n", emp->id, emp->name);
            break;
        }
        case CLIENT: {
            client* cli = (client*)data;
            printf("Client ID: %lld, Name: %s, Account Number: %s, Aadhaar: %s\n",
                   cli->id, cli->name, cli->account_number, cli->aadhaar);
            break;
        }
        case EMPLOYEE_AUTH: {
            employee_auth* emp_auth = (employee_auth*)data;
            printf("Employee Auth Username: %s, Password: %s, Employee ID: %lld\n",
                   emp_auth->username, emp_auth->password, emp_auth->emp_id);
            break;
        }
        case CLIENT_AUTH: {
            client_auth* cli_auth = (client_auth*)data;
            printf("Client Auth Username: %s, Password: %s, Client ID: %lld\n",
                   cli_auth->username, cli_auth->password, cli_auth->client_id);
            break;
        }
        default:
            printf("Unknown table type\n");
            break;
    }

    free(data);
}


void handle_client_request(int client_socket) {
    int request_type;
    enum DB db;
    long long id;
    size_t record_size;

    // Read the request type (1 = insert, 2 = fetch)
    recv(client_socket, &request_type, sizeof(int), 0);
    recv(client_socket, &db, sizeof(enum DB), 0);
    recv(client_socket, &id, sizeof(long long), 0);
    
    record_size = get_record_size(db);

    switch (request_type) {
        case 1: {  // Insert request
            void* record = malloc(record_size);
            recv(client_socket, record, record_size, 0);
            insert_to_db(db, record, record_size);
            free(record);
            break;
        }
        case 2: {  // Fetch request
            fetch_from_db(db, id, record_size);
            break;
        }
        default:
            printf("Unknown request type\n");
    }
    
    close(client_socket);
}

void test_insert_and_fetch() {
    // Initialize the database
    initialize_db();

    // Create a few employee records
    employee emp1 = {1, "John Doe"};
    employee emp2 = {2, "Jane Smith"};
    employee emp3 = {3, "Alice Brown"};

    // Create a few client records
    client client1 = {1, "Client One", "123456789012345", "987654321012"};
    client client2 = {2, "Client Two", "987654321098765", "123456789012"};
    client client3 = {3, "Client Three", "123450987654321", "012345678901"};

    // Insert employees
    printf("Inserting employees...\n");
    insert_to_db(EMPLOYEE, &emp1, sizeof(employee));
    insert_to_db(EMPLOYEE, &emp2, sizeof(employee));
    insert_to_db(EMPLOYEE, &emp3, sizeof(employee));

    // Insert clients
    printf("Inserting clients...\n");
    insert_to_db(CLIENT, &client1, sizeof(client));
    insert_to_db(CLIENT, &client2, sizeof(client));
    insert_to_db(CLIENT, &client3, sizeof(client));

    // Fetch and display employee records
    printf("\nFetching employees...\n");
    employee fetched_emp;
    fetch_from_db(EMPLOYEE, 1, sizeof(employee));
    fetch_from_db(EMPLOYEE, 2, sizeof(employee));
    fetch_from_db(EMPLOYEE, 3, sizeof(employee));

    // Fetch and display client records
    printf("\nFetching clients...\n");
    client fetched_client;
    fetch_from_db(CLIENT, 1, sizeof(client));
    fetch_from_db(CLIENT, 2, sizeof(client));
    fetch_from_db(CLIENT, 3, sizeof(client));
}

int main() {
    test_insert_and_fetch();
    return 0;
}


// int main() {
//     initialize_db();

//     // Set up a basic server to listen for socket requests
//     int server_fd, client_socket;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);

//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket failed");
//         exit(EXIT_FAILURE);
//     }

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(8080);

//     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
//         perror("Bind failed");
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }

//     if (listen(server_fd, 3) < 0) {
//         perror("Listen failed");
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }

//     printf("DBMS is running and listening on port 8080\n");

//     // Continuously listen to socket requests
//     while (1) {
//         if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
//             perror("Accept failed");
//             close(server_fd);
//             exit(EXIT_FAILURE);
//         }

//         // Handle client request (insert or fetch)
//         handle_client_request(client_socket);
//     }

//     close(server_fd);
//     return 0;
// }
