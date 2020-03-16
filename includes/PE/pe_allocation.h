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
    IMAGE_NT_HEADERS32 *pe_header;
    // Array of sections headers
    IMAGE_SECTION_HEADER *section_header;
    char **section_data;
} t_pe;

#endif //SILENT_CRYPT_PE_ALLOCATION_H
