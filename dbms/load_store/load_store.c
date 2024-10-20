#include "load_store.h"
#include "../file_locking/file_lock.h"  
#include <stdio.h>

int load_bytes_to_file(int fd, long long start, void *struct_ptr, size_t struct_size) {
    
    if (lock_file(fd, start, struct_size, LOCK_SHARED) == -1) {
        return -1; 
    }
    
    if (lseek(fd, start, SEEK_SET) == (off_t)-1) {
        perror("Failed to seek");
        unlock_file(fd, start, struct_size);  
        return -1;
    }
    
    ssize_t bytes_read = read(fd, struct_ptr, struct_size);
    if (bytes_read == -1) {
        perror("Failed to read");
        unlock_file(fd, start, struct_size);  
        return -1;
    }

    if (unlock_file(fd, start, struct_size) == -1) {
        return -1;  
    }

    return bytes_read; 
}

int store_bytes_to_file(int fd, long long start, void *struct_ptr, size_t struct_size) {

    if(lock_file(fd, start, struct_size, LOCK_EXCLUSIVE) == -1) {
        return -1;
    }

    if(lseek(fd, start, SEEK_SET) == (off_t)-1) {
        perror("Failed to seek");
        return -1;
    }

    ssize_t bytes_wrote = write(fd, struct_ptr, struct_size);

    if (bytes_wrote == -1) {
        perror("Failed to read");
        unlock_file(fd, start, struct_size);  
        return -1;
    }

    if (unlock_file(fd, start, struct_size) == -1) {
        return -1;  
    }

    return bytes_wrote;
}