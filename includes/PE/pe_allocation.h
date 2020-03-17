//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PE_ALLOCATION_H
#define SILENT_CRYPT_PE_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_struct.h"

typedef struct s_pe {
    IMAGE_DOS_HEADER *dos_header;
    char *dos_stub;
    IMAGE_NT_HEADERS32 *pe_header;
    // Array of sections headers
    IMAGE_SECTION_HEADER *section_header;
    // Array of bytes for each section
    char **section_data;
} t_pe;

int allocate_pe(t_pe **pe, void *file_data, size_t file_data_size);

#endif //SILENT_CRYPT_PE_ALLOCATION_H
