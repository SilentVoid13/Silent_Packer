//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_PACK_ELF_H
#define SILENT_PACKER_PACK_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mhash.h>

int pack_elf(char *file, char *file_data, size_t file_data_size, char *output);

#endif //SILENT_PACKER_PACK_ELF_H
