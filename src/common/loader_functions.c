//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "loader_functions.h"
#include "cipher_functions.h"
#include "file_functions.h"
#include "elf_allocation.h"
#include "pe_allocation.h"
#include "packer_config.h"

#include "log.h"

uint64_t text_data_size64;
uint64_t text_entry_point64;
uint64_t cipher_key64;
uint64_t loader_offset64;

uint32_t text_data_size32;
uint32_t text_entry_point32;
uint32_t cipher_key32;
uint32_t loader_offset32;

unsigned char cipher_key128[17];

// Set globals variables for nasm code
char * patch_loader() {
    char *loader;

    if(packer_config.arch == x32_ARCH) {
        loader = malloc(packer_config.loader_size);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, packer_config.loader_size);

        // Copy the loader asm entry point
        memcpy(loader, packer_config.loader_stub, packer_config.loader_size);

        if(strcmp(packer_config.cipher, "aes128_ecb") == 0) {
            memcpy(loader + packer_config.loader_size - CIPHER_KEY_OFFSET32_AES, cipher_key128, 16);
        }
        else {
            memcpy(loader + packer_config.loader_size - CIPHER_KEY_OFFSET32, &cipher_key32, sizeof(uint32_t));
        }
        memcpy(loader + packer_config.loader_size - TEXT_ENTRY_POINT_OFFSET32, &text_entry_point32, sizeof(uint32_t));
        memcpy(loader + packer_config.loader_size - TEXT_DATA_SIZE_OFFSET32, &text_data_size32, sizeof(uint32_t));
        memcpy(loader + packer_config.loader_size - LOADER_OFFSET_OFFSET32, &loader_offset32, sizeof(uint32_t));
    }
    else {
        loader = malloc(packer_config.loader_size);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, packer_config.loader_size);

        // Copy the loader asm entry point
        memcpy(loader, packer_config.loader_stub, packer_config.loader_size);

        if(strcmp(packer_config.cipher, "aes128_ecb") == 0) {
            memcpy(loader + packer_config.loader_size - CIPHER_KEY_OFFSET64_AES, cipher_key128, 16);
        }
        else {
            memcpy(loader + packer_config.loader_size - CIPHER_KEY_OFFSET64, &cipher_key64, sizeof(uint64_t));
        }
        memcpy(loader + packer_config.loader_size - TEXT_ENTRY_POINT_OFFSET64, &text_entry_point64, sizeof(uint64_t));
        memcpy(loader + packer_config.loader_size - TEXT_DATA_SIZE_OFFSET64, &text_data_size64, sizeof(uint64_t));
        memcpy(loader + packer_config.loader_size - LOADER_OFFSET_OFFSET64, &loader_offset64, sizeof(uint64_t));
    }

    if(packer_config.debug_mode) {
        log_debug("Dumping loader ...");
        dump_to_file("loader.dmp", loader, packer_config.loader_size);
    }

    return loader;
}
