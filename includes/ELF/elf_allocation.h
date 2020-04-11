//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_ELF_ALLOCATION_H
#define SILENT_PACKER_ELF_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <linux/elf-em.h>

// http://man7.org/linux/man-pages/man5/elf.5.html

typedef struct s_elf {
    enum{ELF32, ELF64} s_type;
} t_elf;

typedef struct s_elf64
{
    t_elf type_header;
    Elf64_Ehdr	*elf_header;
    Elf64_Phdr	*prog_header;
    Elf64_Shdr	*section_header;
    // Array of bytes for each section
    char **section_data;
} t_elf64;

typedef struct s_elf32 {
    t_elf type_header;
    Elf32_Ehdr	*elf_header;
    Elf32_Phdr	*prog_header;
    Elf32_Shdr	*section_header;
    // Array of bytes for each section
    char **section_data;
} t_elf32;

int allocate_elf_elf_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_program_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_sections_header(t_elf *elf, void *file_data, size_t file_data_size);
int allocate_elf_sections_data(t_elf *elf, void *file_data, size_t file_data_size);

int allocate_elf(t_elf **elf, void *file_data, size_t file_data_size);


#endif //SILENT_PACKER_ELF_ALLOCATION_H
