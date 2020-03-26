//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_FILE_FUNCTIONS_H
#define SILENT_CRYPT_FILE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mhash.h>

int allocate_file(char *file, void **file_data, size_t *file_data_size);
int write_to_file(int fd, void *data, size_t data_size);
void add_zero_padding(int fd, size_t end_offset);

#endif //SILENT_CRYPT_FILE_FUNCTIONS_H
