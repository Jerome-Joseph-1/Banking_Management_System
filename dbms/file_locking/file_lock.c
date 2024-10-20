#include "file_lock.h"

int lock_file(int fd, long long start, long long length, lock_type_t lock_type) {
    struct flock lock;
    
    memset(&lock, 0, sizeof(lock));
    lock.l_type = lock_type; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = start; 
    lock.l_len = length; 
    
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Failed to lock file");
        return -1;
    }

    return 0;
}

int unlock_file(int fd, long long start, long long length) {
    return lock_file(fd, start, length, LOCK_UNLOCK);
}

