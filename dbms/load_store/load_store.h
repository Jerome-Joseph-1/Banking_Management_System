#ifndef LOAD_STORE_H
#define LOAD_STORE_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int load_bytes_to_file(int fd, long long start, void *struct_ptr, size_t struct_size);
int store_bytes_to_file(int fd, long long start, void *struct_ptr, size_t struct_size);
#endif