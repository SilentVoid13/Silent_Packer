//
// Created by silentvoid on 3/7/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_CODE_CAVE_H
#define SILENT_CRYPT_CODE_CAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

int find_elf_code_cave_index(t_elf *elf);
int set_new_elf_cave_segment_values(t_elf *elf, int segment_index);
int elf_cave_insert_loader(t_elf *elf, int section_index, int old_section_size);

int elf_code_cave_injection(t_elf *elf);

#endif //SILENT_CRYPT_CODE_CAVE_H
