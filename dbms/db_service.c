#include <pthread.h>
#include <sys/stat.h>
#include "load_store/load_store.h"
#include "schema/schema.h"
#include "../data_structs/map_vector/map_vector.h"
#include "../data_structs/vector/vector.h"
#include "index/index_management.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080
#define BUFFER_SIZE 1024

map_vector* indexes;  // Updated to use the new map_vector structure
vector* db_fds;  // Vector to store file descriptors for each DB

void set_map(map_vector* indexes, enum DB db, map_i* db_index) {
    if (db < 0 || db >= indexes->size) {
        fprintf(stderr, "Invalid table index for DB enum: %d\n", db);
        return;
    }

    // Set the map at the corresponding index (db) in the vector
    set_map_vector(indexes, db, db_index);
}

void initialize_db() {
    size_t table_count = 4;  // Example: 4 tables (CLIENT, CLIENT_AUTH, EMPLOYEE, EMPLOYEE_AUTH)
    indexes = make_map_vector(table_count);
    db_fds = make_vector(table_count);

    // Initialize indexes for the different tables
    initialize_indexes(indexes);

    // Open database files and load the indexes
    for (int i = 0; i < table_count; i++) {
        enum DB db = (enum DB)i;

        // Construct filenames for DB files
        char db_filename[256];
        snprintf(db_filename, sizeof(db_filename), "db/%d.db", db);

        // Open the database file and store the file descriptor in the vector
        int fd = open(db_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("Failed to open database file");
            exit(EXIT_FAILURE);
        }
        push_back(db_fds, fd);

        // Rebuild the index for the current table
        rebuild_indexes(db, indexes);
    }
}

int get_fd_of_db(enum DB db) {
    // Retrieve the file descriptor for the given DB from the vector
    return vector_get(db_fds, db);
}

void insert_to_db(enum DB db, void* data_struct, size_t size) {
    int fd = get_fd_of_db(db);
    void* db_index = get_map(indexes, db);

    long long id = extract_id_from_record(db, data_struct);
    bool found = false;
    long long start_position;

    // Determine the correct index based on the DB type
    if (db == CLIENT) {
        client_indexes* ci = (client_indexes*)db_index; // Cast to client_indexes
        start_position = find_i(ci->id_index, id, &found);
    } else if (db == EMPLOYEE) {
        employee_indexes* ei = (employee_indexes*)db_index; // Cast to employee_indexes
        start_position = find_i(ei->id_index, id, &found);
    } else {
        printf("Invalid DB type for insertion: %d\n", db);
        return;
    }

    if (!found) {
        start_position = lseek(fd, 0, SEEK_END);  // Append at the end if not found
        if (db == CLIENT) {
            insert_i(((client_indexes*)db_index)->id_index, id, start_position);
        } else if (db == EMPLOYEE) {
            insert_i(((employee_indexes*)db_index)->id_index, id, start_position);
        }
    }

    // Store the record in the DB file
    store_bytes_to_file(fd, start_position, data_struct, size);
}

void* fetch_from_db(enum DB db, long long id, size_t size) {
    int fd = get_fd_of_db(db);
    
    // Get the correct index based on the DB type
    void* db_index = get_map(indexes, db); // This now returns a generic pointer

    bool found = false;
    long long start_position;

    // Determine if we're dealing with client or employee and find the correct position
    if (db == CLIENT) {
        client_indexes* ci = (client_indexes*)db_index; // Cast to client_indexes
        start_position = find_i(ci->id_index, id, &found);
    } else if (db == EMPLOYEE) {
        employee_indexes* ei = (employee_indexes*)db_index; // Cast to employee_indexes
        start_position = find_i(ei->id_index, id, &found);
    } else {
        printf("Invalid DB type for fetching: %d\n", db);
        return NULL;
    }

    if (!found) {
        printf("Record not found for ID %lld\n", id);
        return NULL;
    }

    // Allocate memory for the data
    void* data = malloc(size);
    if (!data) {
        perror("Failed to allocate memory for data");
        return NULL;
    }

    // Load data from the file
    load_bytes_to_file(fd, start_position, data, size);

    return data;  // Return the fetched data
}

void update_in_db_partial(enum DB db, long long id, void* partial_data, size_t size) {
    // Step 1: Fetch the existing record
    void* existing_record = fetch_from_db(db, id, size);
    if (existing_record == NULL) {
        printf("Record with ID %lld not found in database.\n", id);
        return;
    }

    // Step 2: Apply partial updates (based on the DB type)
    switch (db) {
        case EMPLOYEE: {
            employee* emp_existing = (employee*)existing_record;
            employee* emp_update = (employee*)partial_data;

            // Only update fields that are populated (non-zero/non-null)
            if (emp_update->name[0] != '\0') {
                strncpy(emp_existing->name, emp_update->name, sizeof(emp_existing->name));
            }
            break;
        }

        case CLIENT: {
            client* cli_existing = (client*)existing_record;
            client* cli_update = (client*)partial_data;

            // Update only fields that are populated in the partial update
            if (cli_update->name[0] != '\0') {
                strncpy(cli_existing->name, cli_update->name, sizeof(cli_existing->name));
            }
            if (cli_update->account_number[0] != '\0') {
                strncpy(cli_existing->account_number, cli_update->account_number, sizeof(cli_existing->account_number));
            }
            if (cli_update->aadhaar[0] != '\0') {
                strncpy(cli_existing->aadhaar, cli_update->aadhaar, sizeof(cli_existing->aadhaar));
            }
            break;
        }

        default:
            printf("Unknown DB type for partial update.\n");
            free(existing_record);
            return;
    }

    // Step 3: Re-insert the updated record back into the database
    insert_to_db(db, existing_record, size);

    // Step 4: Free the fetched record (if necessary) and return the result
    free(existing_record);
}

// Function to handle incoming requests
void handle_client_request(int new_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int valread = read(new_socket, buffer, BUFFER_SIZE);

    // Parse the request (you can define a more structured protocol if needed)
    char command[BUFFER_SIZE];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "INSERT") == 0) {
        // Example: INSERT CLIENT {client data in binary or JSON format}
        // Parse data and call insert_to_db
        client new_client;
        long long id;
        sscanf(buffer + strlen("INSERT CLIENT "), "%lld %s %s %s", &new_client.id, new_client.name, new_client.account_number, new_client.aadhaar);

        insert_to_db(CLIENT, &new_client, sizeof(client));

        char* response = "Insert successful";
        send(new_socket, response, strlen(response), 0);

    } else if (strcmp(command, "FETCH") == 0) {
        // Example: FETCH CLIENT {id}
        // Parse the request to fetch data
        long long id;
        sscanf(buffer + strlen("FETCH CLIENT "), "%lld", &id);

        client* fetched_client = (client*)fetch_from_db(CLIENT, id, sizeof(client));
        if (fetched_client) {
            // You can send the fetched client data back as JSON, or any format you prefer
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Fetched Client: Name: %s, Account: %s, Aadhaar: %s\n", fetched_client->name, fetched_client->account_number, fetched_client->aadhaar);
            send(new_socket, response, strlen(response), 0);
            free(fetched_client);
        } else {
            char* response = "Client not found";
            send(new_socket, response, strlen(response), 0);
        }

    } else if (strcmp(command, "UPDATE") == 0) {
        // Example: UPDATE CLIENT {client id} {new client data}
        // Parse data and call update_in_db_partial
        client update_client;
        long long id;
        sscanf(buffer + strlen("UPDATE CLIENT "), "%lld %s", &id, update_client.name);  // For example, update only the name

        update_in_db_partial(CLIENT, id, &update_client, sizeof(client));

        char* response = "Update successful";
        send(new_socket, response, strlen(response), 0);

    } else {
        char* response = "Invalid command";
        send(new_socket, response, strlen(response), 0);
    }
}

// Start the server
void start_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    printf("Server started and listening on port %d...\n", PORT);

    // Main loop: accept connections and handle requests
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted\n");

        // Handle the client's request
        handle_client_request(new_socket);

        // Close the socket after handling the request
        close(new_socket);
    }
}

int main() {
    // Initialize the database and load indexes
    initialize_db();

    // Start the socket server
    start_server();

    return 0;
}
