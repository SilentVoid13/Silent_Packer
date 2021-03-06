//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_ELF_FUNCTIONS_H
#define SILENT_PACKER_ELF_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

int set_new_elf_entry_to_section(t_elf *elf, int section_index);
int set_new_elf_entry_to_addr32(t_elf *elf, int32_t entry_address, int section_index, int section_size);
int set_new_elf_entry_to_addr64(t_elf *elf, int64_t entry_address, int section_index, int section_size);

int find_last_elf_segment_of_type(t_elf *elf, unsigned int p_type);
int find_last_elf_section_from_segment(t_elf *elf, int segment_index);
int find_elf_text_segment(t_elf *elf);
int find_elf_segment_index_of_section(t_elf *elf, int section_index);
char * find_elf_section_name(t_elf *elf, int index);
int find_elf_section_index(t_elf *elf, char *section_name);

void add_elf_segment_permission(t_elf *elf, int segment_index, int permission);

#endif //SILENT_PACKER_ELF_FUNCTIONS_H
