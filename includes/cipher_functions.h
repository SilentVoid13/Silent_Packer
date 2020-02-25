//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_CIPHER_FUNCTIONS_H
#define SILENT_CRYPT_CIPHER_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "elf_allocation.h"

int encrypt_data(t_elf *elf, char *cipher);

#endif //SILENT_CRYPT_CIPHER_FUNCTIONS_H
