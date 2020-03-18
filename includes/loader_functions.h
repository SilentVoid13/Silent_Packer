//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_LOADER_FUNCTIONS_H
#define SILENT_CRYPT_LOADER_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CIPHER_KEY_OFFSET 24
#define TEXT_ENTRY_POINT_OFFSET 16
#define TEXT_DATA_SIZE_OFFSET 8

// TODO: Maybe add this to a big struct loader_config
extern size_t loader_size;
extern size_t infos_size;
extern void loader_entry_point(void);

char * patch_loader();

#endif //SILENT_CRYPT_LOADER_FUNCTIONS_H
