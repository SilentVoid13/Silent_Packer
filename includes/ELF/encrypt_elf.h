//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_ENCRYPT_ELF_H
#define SILENT_CRYPT_ENCRYPT_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

int encrypt_elf(t_elf *elf, char *cipher);

#endif //SILENT_CRYPT_ENCRYPT_ELF_H
