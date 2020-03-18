//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACKING_METHOD_ELF_H
#define SILENT_CRYPT_PACKING_METHOD_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

#include "packing_method.h"

int pack_using_method(t_elf *elf, char *packing_method);

#endif //SILENT_CRYPT_PACKING_METHOD_ELF_H
