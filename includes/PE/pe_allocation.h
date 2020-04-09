//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_PE_ALLOCATION_H
#define SILENT_PACKER_PE_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_struct.h"

typedef struct s_pe {
    enum{PE32, PE64} s_type;
} t_pe;

typedef struct s_pe32 {
    t_pe type_header;
    IMAGE_DOS_HEADER *dos_header;
    char *dos_stub;
    IMAGE_NT_HEADERS32 *pe_header;
    IMAGE_SECTION_HEADER *section_header;
    char **section_data;
} t_pe32;

typedef struct s_pe64 {
    t_pe type_header;
    IMAGE_DOS_HEADER *dos_header;
    char *dos_stub;
    IMAGE_NT_HEADERS64 *pe_header;
    IMAGE_SECTION_HEADER *section_header;
    char **section_data;
} t_pe64;

/*
 * Can be a solution too
typedef struct s_pe {
    enum{PE32, PE64} s_type;

    union {
        t_pe64 pe64;
        t_pe32 pe32;
    };
} t_pe;
*/

int allocate_pe_dos_header(t_pe *pe, void *file_data, size_t file_data_size);
int allocate_pe_dos_stub(t_pe *pe, void *file_data);
int allocate_pe_pe_header(t_pe *pe, void *file_data, size_t file_data_size);
int allocate_pe_sections_headers(t_pe *pe, void *file_data, size_t file_data_size);
int allocate_pe_sections_data(t_pe *pe, void *file_data, size_t file_data_size);
void print_pe_info(t_pe *pe);

int allocate_pe(t_pe **pe, void *file_data, size_t file_data_size, int arch);

#endif //SILENT_PACKER_PE_ALLOCATION_H
