//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_PACKING_METHOD_ELF_H
#define SILENT_PACKER_PACKING_METHOD_ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf_allocation.h"

#include "packing_method.h"

int elf_pack_using_method(t_elf *elf);

#endif //SILENT_PACKER_PACKING_METHOD_ELF_H
