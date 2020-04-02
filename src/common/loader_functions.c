//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "loader_functions.h"
#include "cipher_functions.h"
#include "file_functions.h"
#include "elf_allocation.h"
#include "pe_allocation.h"
#include "all_loaders.h"

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
char * patch_loader(int arch, int file_type) {
    char *loader;
    size_t loader_size;
    unsigned char *loader_stub;

    if(arch == x32_ARCH) {
        if(file_type == ELF32) {
            loader_size = I386_LINUX_ELF_LOADER_SIZE;
            loader_stub = i386_linux_elf_loader_stub;
        }
        else if(file_type == PE32) {
            loader_size = I386_WIN_PE_LOADER_SIZE;
            loader_stub = i386_win_pe_loader_stub;
        }
        else {
            log_error("Unknown file type");
            return NULL;
        }

        loader = malloc(loader_size);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, loader_size);

        // Copy the loader asm entry point
        memcpy(loader, loader_stub, loader_size);

        printf("loader_size: %ld\n", loader_size);
        printf("loader_offset: %x\n", loader_offset32);
        printf("text_entry_point : %x\n", text_entry_point32);

        memcpy(loader + loader_size - CIPHER_KEY_OFFSET32, &cipher_key32, sizeof(uint32_t));
        memcpy(loader + loader_size - TEXT_ENTRY_POINT_OFFSET32, &text_entry_point32, sizeof(uint32_t));
        memcpy(loader + loader_size - TEXT_DATA_SIZE_OFFSET32, &text_data_size32, sizeof(uint32_t));
        memcpy(loader + loader_size - LOADER_OFFSET_OFFSET32, &loader_offset32, sizeof(uint32_t));

        FILE *dump;
        dump = fopen("dump", "w");
        fwrite(loader, loader_size, 1, dump);
        fclose(dump);
    }
    else {
        if(file_type == ELF64) {
            loader_size = AMD64_LINUX_ELF_LOADER_SIZE;
            loader_stub = amd64_linux_elf_loader_stub;
        }
        else if(file_type == PE64) {
            loader_size = AMD64_WIN_PE_LOADER_SIZE;
            loader_stub = amd64_win_pe_loader_stub;
        }
        else {
            log_error("Unknown file type");
            return NULL;
        }

        loader = malloc(loader_size);
        if(loader == NULL) {
            log_error("malloc() failure");
            return NULL;
        }
        memset(loader, 0x0, loader_size);

        printf("loader_size: %ld\n", loader_size);

        // Copy the loader asm entry point
        memcpy(loader, loader_stub, loader_size);

        memcpy(loader + loader_size - CIPHER_KEY_OFFSET64, &cipher_key64, sizeof(uint64_t));
        memcpy(loader + loader_size - TEXT_ENTRY_POINT_OFFSET64, &text_entry_point64, sizeof(uint64_t));
        memcpy(loader + loader_size - TEXT_DATA_SIZE_OFFSET64, &text_data_size64, sizeof(uint64_t));
        memcpy(loader + loader_size - LOADER_OFFSET_OFFSET64, &loader_offset64, sizeof(uint64_t));
    }

    return loader;
}
