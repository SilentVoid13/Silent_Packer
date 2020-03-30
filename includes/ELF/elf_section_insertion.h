//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_SECTION_INSERTION_H
#define SILENT_CRYPT_SECTION_INSERTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ELF/elf_allocation.h"

Elf64_Shdr new_section = {
        .sh_name = 0,
        .sh_type = SHT_PROGBITS,
        .sh_flags = SHF_EXECINSTR | SHF_ALLOC, // NOLINT(hicpp-signed-bitwise)
        .sh_addr = 0,
        .sh_offset = 0,
        .sh_size = 0,
        .sh_link = 0,
        .sh_info = 0,
        .sh_addralign = 16,
        .sh_entsize = 0,
};

int set_new_elf_section_string_table(t_elf *elf);
int set_new_elf_section_symtab_sh_link_value(t_elf *elf);
int elf_section_create_new_section(t_elf *elf, int last_pt_load_index, int last_loadable_section_index);
void print_link_fields(t_elf *elf);
int set_new_elf_section_pt_loader_permissions(t_elf *elf);

int elf_insert_section(t_elf *elf);

#endif //SILENT_CRYPT_SECTION_INSERTION_H
