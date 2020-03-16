//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "file_allocation.h"

#include "log.h"

/*
 * Mapping file in memory using mmap
 *
 */
int allocate_file(char *file, void **file_data, size_t *file_data_size) {
    int fd = open(file, O_RDONLY);
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    int size = lseek(fd, 0, SEEK_END);
    if(size < 0) {
        log_error("lseek() failure");
        return -1;
    }
    *file_data_size = size;

    *file_data = mmap(NULL, *file_data_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); // NOLINT(hicpp-signed-bitwise)
    if(*file_data == MAP_FAILED) {
        log_error("mmap() failure");
        return -1;
    }
    close(fd);

    return 1;
}
