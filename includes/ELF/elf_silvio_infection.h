//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_SILVIO_INFECTION_H
#define SILENT_CRYPT_SILVIO_INFECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ELF/elf_allocation.h"

#define PAGE_SIZE64 0x2000
#define PAGE_SIZE32 4096

int set_new_elf_silvio_segment_values(t_elf *elf, int text_segment_index);
int set_new_elf_silvio_section_values(t_elf *elf, int last_section_index);
int elf_silvio_insert_loader(t_elf *elf, int section_index, int old_section_size);

int elf_silvio_infect(t_elf *elf);

#endif //SILENT_CRYPT_SILVIO_INFECTION_H
