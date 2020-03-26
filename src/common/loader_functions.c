//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "loader_functions.h"
#include "cipher_functions.h"

#include "log.h"

uint64_t text_data_size;
uint64_t text_entry_point;
uint64_t cipher_key;
uint64_t loader_offset;

// Set globals variables for nasm code
char * patch_loader() {
    char *loader = malloc(loader_size);
    if(loader == NULL) {
        log_error("malloc() failure");
        return NULL;
    }
    memset(loader, 0x0, loader_size);

    // Copy the loader asm entry point
    memcpy(loader, loader_entry_point, loader_size);

    // Variables are 8 bytes 64 bit addresses (uint64_t)
    memcpy(loader + loader_size - CIPHER_KEY_OFFSET, &cipher_key, sizeof(uint64_t));
    memcpy(loader + loader_size - TEXT_ENTRY_POINT_OFFSET, &text_entry_point, sizeof(uint64_t));
    memcpy(loader + loader_size - TEXT_DATA_SIZE_OFFSET, &text_data_size, sizeof(uint64_t));
    memcpy(loader + loader_size - LOADER_OFFSET_OFFSET, &loader_offset, sizeof(uint64_t));

    return loader;
}
