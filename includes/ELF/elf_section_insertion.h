//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_SECTION_INSERTION_H
#define SILENT_CRYPT_SECTION_INSERTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

int set_new_elf_section_string_table(t_elf *elf);
int set_new_elf_section_symtab_sh_link_value(t_elf *elf);
int elf_section_create_new_section(t_elf *elf, int last_pt_load_index, int last_loadable_section_index);
void print_link_fields(t_elf *elf);
int set_new_elf_section_pt_loader_permissions(t_elf *elf);

int elf_insert_section(t_elf *elf);

#endif //SILENT_CRYPT_SECTION_INSERTION_H
