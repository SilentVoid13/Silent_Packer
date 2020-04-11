//
// Created by silentvoid on 4/10/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "packer_config.h"
#include "all_loaders.h"
#include "cipher_functions.h"
#include "file_functions.h"
#include "elf_allocation.h"
#include "pe_allocation.h"

#include "log.h"

struct s_packer_config packer_config;

int fill_packer_config(char *packing_method, char *cipher, int arch, int file_type, int debug_mode) {
    strncpy(packer_config.packing_method, packing_method, MAX_PACKING_METHOD_LENGTH);
    strncpy(packer_config.cipher, cipher, MAX_CIPHER_LENGTH);
    packer_config.arch = arch;
    packer_config.debug_mode = debug_mode;

    if(arch == x32_ARCH) {
        if (file_type == ELF_FILE) {
            if (strcmp(cipher, "aes128_ecb") == 0) {
                log_error("AES-128-ECB is not currently available for ELF 32 bit");
                return -1;
                //packer_config.loader_infos_size = I386_AES_128_ECB_LINUX_ELF_LOADER_INFOS_SIZE;
                //packer_config.loader_size = I386_AES_128_ECB_LINUX_ELF_LOADER_SIZE;
                //packer_config.loader_stub = &(i386_aes_128_ecb_linux_elf_loader_stub[0]);
            } else if (strcmp(cipher, "xor") == 0) {
                packer_config.loader_infos_size = I386_XOR_LINUX_ELF_LOADER_INFOS_SIZE;
                packer_config.loader_size = I386_XOR_LINUX_ELF_LOADER_SIZE;
                packer_config.loader_stub = &(i386_xor_linux_elf_loader_stub[0]);
            }
            else {
                log_error("Unknown cipher");
                return -1;
            }
        } else if (file_type == PE_FILE) {
            if (strcmp(cipher, "aes128_ecb") == 0) {
                log_error("AES-128-ECB is not currently available for PE files");
                return -1;
                //packer_config.loader_infos_size = I386_AES_128_ECB_WIN_PE_LOADER_INFOS_SIZE;
                //packer_config.loader_size = I386_AES_128_ECB_WIN_PE_LOADER_SIZE;
                //packer_config.loader_stub = &(i386_aes_128_ecb_win_pe_loader_stub[0]);
            } else if (strcmp(cipher, "xor") == 0) {
                packer_config.loader_infos_size = I386_XOR_WIN_PE_LOADER_INFOS_SIZE;
                packer_config.loader_size = I386_XOR_WIN_PE_LOADER_SIZE;
                packer_config.loader_stub = &(i386_xor_win_pe_loader_stub[0]);
            }
            else {
                log_error("Unknown cipher");
                return -1;
            }
        } else {
            log_error("Unknown file type");
            return -1;
        }
        return 1;
    }
    else if(arch == x64_ARCH) {
        if(file_type == ELF_FILE) {
            if(strcmp(cipher, "aes128_ecb") == 0) {
                packer_config.loader_infos_size = AMD64_AES_128_ECB_LINUX_ELF_LOADER_INFOS_SIZE;
                packer_config.loader_size = AMD64_AES_128_ECB_LINUX_ELF_LOADER_SIZE;
                packer_config.loader_stub = &(amd64_aes_128_ecb_linux_elf_loader_stub[0]);
            }
            else if(strcmp(cipher, "xor") == 0) {
                packer_config.loader_infos_size = AMD64_XOR_LINUX_ELF_LOADER_INFOS_SIZE;
                packer_config.loader_size = AMD64_XOR_LINUX_ELF_LOADER_SIZE;
                packer_config.loader_stub = &(amd64_xor_linux_elf_loader_stub[0]);
            }
            else {
                log_error("Unknown cipher");
                return -1;
            }
        }
        else if(file_type == PE_FILE) {
            if(strcmp(cipher, "aes128_ecb") == 0) {
                log_error("AES-128-ECB is not currently available for PE files");
                return -1;
                //packer_config.loader_infos_size = AMD64_AES_128_ECB_WIN_PE_LOADER_INFOS_SIZE;
                //packer_config.loader_size = AMD64_AES_128_ECB_WIN_PE_LOADER_SIZE;
                //packer_config.loader_stub = amd64_aes_128_ecb_win_pe_loader_stub;
            }
            else if(strcmp(cipher, "xor") == 0) {
                packer_config.loader_infos_size = AMD64_XOR_WIN_PE_LOADER_INFOS_SIZE;
                packer_config.loader_size = AMD64_XOR_WIN_PE_LOADER_SIZE;
                packer_config.loader_stub = &(amd64_xor_win_pe_loader_stub[0]);
            }
            else {
                log_error("Unknown cipher");
                return -1;
            }
        }
        else {
            log_error("Unknown file type");
            return -1;
        }
    }

    return 1;
}

