//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_FILE_ALLOCATION_H
#define SILENT_CRYPT_FILE_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mhash.h>

int allocate_file(char *file, void **file_data, size_t *file_data_size);

#endif //SILENT_CRYPT_FILE_ALLOCATION_H
