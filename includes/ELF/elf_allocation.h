//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_ELF_ALLOCATION_H
#define SILENT_CRYPT_ELF_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <linux/elf-em.h>

// Elf binary map
typedef struct s_elf
{
    Elf64_Ehdr	*elf_header;
    Elf64_Phdr	*prog_header;
    Elf64_Shdr	*section_header;
    // Array of bytes for each section
    char **section_data;
} t_elf;

int allocate_elf_elf_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_program_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_sections_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_sections_data(t_elf *elf, void *file_data, size_t file_data_size);

int allocate_elf(t_elf **elf, void *file_data, size_t file_data_size);


#endif //SILENT_CRYPT_ELF_ALLOCATION_H
