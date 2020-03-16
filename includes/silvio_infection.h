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

int silvio_infect(t_elf *elf);

#endif //SILENT_CRYPT_SILVIO_INFECTION_H
