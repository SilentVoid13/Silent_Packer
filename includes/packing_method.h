//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACKING_METHOD_H
#define SILENT_CRYPT_PACKING_METHOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ELF/elf_allocation.h"

#define CIPHER_KEY_OFFSET 24
#define TEXT_ENTRY_POINT_OFFSET 16
#define TEXT_DATA_SIZE_OFFSET 8

#define SECTION_INSERTION_METHOD 1
#define SILVIO_INFECTION_METHOD 2
#define CODE_CAVE_METHOD 3


// TODO: Maybe add this to a big struct loader_config
extern size_t loader_size;
extern size_t infos_size;
extern void loader_entry_point(void);

struct method_config {
    int method_type;
    int concerned_section;
};

extern struct method_config method_config;

int pack_using_method(t_elf *elf, char *packing_method);

#endif //SILENT_CRYPT_PACKING_METHOD_H
