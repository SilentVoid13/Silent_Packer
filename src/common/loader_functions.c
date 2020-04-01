//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "loader_functions.h"
#include "cipher_functions.h"
#include "file_functions.h"

#include "log.h"

uint64_t text_data_size64;
uint64_t text_entry_point64;
uint64_t cipher_key64;
uint64_t loader_offset64;

uint32_t text_data_size32;
uint32_t text_entry_point32;
uint32_t cipher_key32;
uint32_t loader_offset32;

// Set globals variables for nasm code
char * patch_loader(int arch) {
    char *loader;
    if(arch == x32_ARCH) {
        loader = malloc(loader_size32);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, loader_size32);

        // Copy the loader asm entry point
        memcpy(loader, loader_entry_point32, loader_size32);

        printf("loader_offset: %x\n", loader_offset32);
        printf("text_entry_point : %x\n", text_entry_point32);

        memcpy(loader + loader_size32 - CIPHER_KEY_OFFSET, &cipher_key32, sizeof(uint32_t));
        memcpy(loader + loader_size32 - TEXT_ENTRY_POINT_OFFSET, &text_entry_point32, sizeof(uint32_t));
        memcpy(loader + loader_size32 - TEXT_DATA_SIZE_OFFSET, &text_data_size32, sizeof(uint32_t));
        memcpy(loader + loader_size32 - LOADER_OFFSET_OFFSET, &loader_offset32, sizeof(uint32_t));
    }
    else {
        loader = malloc(loader_size64);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, loader_size64);

        // Copy the loader asm entry point
        memcpy(loader, loader_entry_point64, loader_size64);

        memcpy(loader + loader_size64 - CIPHER_KEY_OFFSET, &cipher_key64, sizeof(uint64_t));
        memcpy(loader + loader_size64 - TEXT_ENTRY_POINT_OFFSET, &text_entry_point64, sizeof(uint64_t));
        memcpy(loader + loader_size64 - TEXT_DATA_SIZE_OFFSET, &text_data_size64, sizeof(uint64_t));
        memcpy(loader + loader_size64 - LOADER_OFFSET_OFFSET, &loader_offset64, sizeof(uint64_t));
    }

    return loader;
}
