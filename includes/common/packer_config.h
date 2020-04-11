//
// Created by silentvoid on 4/10/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_PACKER_CONFIG_H
#define SILENT_PACKER_PACKER_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_CIPHER_LENGTH 20
#define MAX_PACKING_METHOD_LENGTH 40

struct s_packer_config {
    int arch;
    char cipher[MAX_CIPHER_LENGTH];
    char packing_method[MAX_PACKING_METHOD_LENGTH];
    size_t cipher_key_offset;
    size_t loader_size;
    size_t loader_infos_size;
    unsigned char *loader_stub;
    int debug_mode;
};

extern struct s_packer_config packer_config;

int fill_packer_config(char *packing_method, char *cipher, int arch, int file_type, int debug_mode);

#endif //SILENT_PACKER_PACKER_CONFIG_H
