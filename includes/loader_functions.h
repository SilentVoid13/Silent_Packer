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

#define CIPHER_KEY_OFFSET 32
#define TEXT_ENTRY_POINT_OFFSET 24
#define TEXT_DATA_SIZE_OFFSET 16
#define LOADER_OFFSET_OFFSET 8

// TODO: Maybe add this to a big struct loader_config
extern size_t loader_size32;
extern size_t infos_size32;
extern void loader_entry_point32(void);

extern size_t loader_size64;
extern size_t infos_size64;
extern void loader_entry_point64(void);

extern uint32_t text_data_size32;
extern uint32_t text_entry_point32;
extern uint32_t cipher_key32;
extern uint32_t loader_offset32;

extern uint64_t text_data_size64;
extern uint64_t text_entry_point64;
extern uint64_t cipher_key64;
extern uint64_t loader_offset64;

char * patch_loader(int arch);

#endif //SILENT_CRYPT_LOADER_FUNCTIONS_H
