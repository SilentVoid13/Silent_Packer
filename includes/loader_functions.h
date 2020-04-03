//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_LOADER_FUNCTIONS_H
#define SILENT_CRYPT_LOADER_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CIPHER_KEY_OFFSET32         16
#define TEXT_ENTRY_POINT_OFFSET32   12
#define TEXT_DATA_SIZE_OFFSET32     8
#define LOADER_OFFSET_OFFSET32      4

#define CIPHER_KEY_OFFSET64         32
#define TEXT_ENTRY_POINT_OFFSET64   24
#define TEXT_DATA_SIZE_OFFSET64     16
#define LOADER_OFFSET_OFFSET64      8

extern uint32_t text_data_size32;
extern uint32_t text_entry_point32;
extern uint32_t cipher_key32;
extern uint32_t loader_offset32;

extern uint64_t text_data_size64;
extern uint64_t text_entry_point64;
extern uint64_t cipher_key64;
extern uint64_t loader_offset64;

char * patch_loader(int arch, int file_type);

#endif //SILENT_CRYPT_LOADER_FUNCTIONS_H
