//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_WRITE_ELF_H
#define SILENT_CRYPT_WRITE_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ELF/elf_allocation.h"

int write_elf(t_elf *elf, char *filename);

#endif //SILENT_CRYPT_WRITE_ELF_H
