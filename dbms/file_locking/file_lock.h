#ifndef FILE_LOC_H
#define FILE_LOC_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef enum {
    LOCK_SHARED = F_RDLCK,  
    LOCK_EXCLUSIVE = F_WRLCK, 
    LOCK_UNLOCK = F_UNLCK  
} lock_type_t;

int lock_file(int fd, long long start, long long length, lock_type_t lock_type);
int unlock_file(int fd, long long start, long long length);

#endif