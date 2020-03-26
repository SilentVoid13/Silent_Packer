//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_ELF_FUNCTIONS_H
#define SILENT_CRYPT_ELF_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ELF/elf_allocation.h"

int set_new_elf_entry_to_section(t_elf *elf, int section_index);
int find_last_segment_of_type(t_elf *elf, unsigned int p_type);
int find_last_section(t_elf *elf, int segment_index);
int find_last_section_from_segment(t_elf *elf, int segment_index);
int find_text_segment(t_elf *elf);
int find_segment_index_of_section(t_elf *elf, int section_index);
int set_new_elf_entry_to_addr(t_elf *elf, int64_t entry_address, int section_index, int section_size);
char* get_section_name(t_elf *elf, int index);
int get_section_index(t_elf *elf, char *section_name);
void add_segment_permission(t_elf *elf, int segment_index, int permission);

#endif //SILENT_CRYPT_ELF_FUNCTIONS_H
