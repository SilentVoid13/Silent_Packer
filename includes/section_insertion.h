//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_SECTION_INSERTION_H
#define SILENT_CRYPT_SECTION_INSERTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>

#include "elf_allocation.h"

#define CIPHER_KEY_OFFSET 24
#define TEXT_ENTRY_POINT_OFFSET 16
#define TEXT_DATA_SIZE_OFFSET 8

int insert_section(t_elf *elf);

extern size_t loader_size;
extern size_t infos_size;
extern void loader_entry_point(void);

#endif //SILENT_CRYPT_SECTION_INSERTION_H
