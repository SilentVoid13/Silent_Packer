//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACK_PE_H
#define SILENT_CRYPT_PACK_PE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pack_pe(char *file, char *file_data, size_t file_data_size, int arch, char *cipher, char *packing_method, char *output);

#endif //SILENT_CRYPT_PACK_PE_H
