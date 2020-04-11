//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_encryption.h"
#include "cipher_functions.h"
#include "elf_allocation.h"
#include "elf_functions.h"
#include "loader_functions.h"
#include "packer_config.h"
#include "aes_128_ecb_encryption.h"

#include "log.h"

int encrypt_elf(t_elf *elf) {
    int text_section_index = find_elf_section_index(elf, ".text");
    if(text_section_index == -1) {
        log_error("Couldn't find .text section in the ELF binary");
        return -1;
    }

    log_verbose("Got .text section index : %d", text_section_index);

    char *text_data;
    if(elf->s_type == ELF32) {
        text_data = ((t_elf32 *)elf)->section_data[text_section_index];

        // Setting global variables
        text_data_size32 = ((t_elf32 *)elf)->section_header[text_section_index].sh_size;
        text_entry_point32 = ((t_elf32 *)elf)->section_header[text_section_index].sh_addr;

        FILE *dump;
        if(packer_config.debug_mode) {
            log_debug("Dumping .text ...");
            dump = fopen("text32.dmdddf", "w");
            fwrite(text_data, text_data_size32, 1, dump);
            fclose(dump);
        }

        log_verbose("Generating random key ...");

        if(strcmp(packer_config.cipher, "xor") == 0) {
            cipher_key32 = generate_random_key32();
            uint32_t temp_key = cipher_key32;
            log_info("Random key : %x", cipher_key32);
            xor_encrypt32(text_data, text_data_size32, temp_key);
        }
        else if(strcmp(packer_config.cipher, "aes128_ecb") == 0) {
            cipher_key128 = generate_random_key128();
            if(cipher_key128 == NULL) {
                log_error("Error during key generation");
                return -1;
            }
            log_info("Random key : ");
            for(int i = 0; i < 16; i++)
                printf("%02x", cipher_key128[i]);
            puts("");

            aes_128_ecb_encrypt(text_data, text_data_size32, cipher_key128, 16);
        }

        if(packer_config.debug_mode) {
            log_debug("Dumping encrypted .text ...");
            dump = fopen("text_encrypted32.dmp", "w");
            fwrite(text_data, text_data_size32, 1, dump);
            fclose(dump);
        }
    }
    else {
        text_data = ((t_elf64 *)elf)->section_data[text_section_index];

        // Setting global variables
        text_data_size64 = ((t_elf64 *)elf)->section_header[text_section_index].sh_size;
        text_entry_point64 = ((t_elf64 *)elf)->section_header[text_section_index].sh_addr;

        FILE *dump;
        if(packer_config.debug_mode) {
            log_debug("Dumping .text ...");
            dump = fopen("text64.dmp", "w");
            fwrite(text_data, text_data_size64, 1, dump);
            fclose(dump);
        }

        log_verbose("Generating random key ...");

        if(strcmp(packer_config.cipher, "xor") == 0) {
            cipher_key64 = generate_random_key64();
            uint64_t temp_key = cipher_key64;
            log_info("Random key : %lx", cipher_key64);
            xor_encrypt64(text_data, text_data_size64, temp_key);
        }
        else if(strcmp(packer_config.cipher, "aes128_ecb") == 0) {
            cipher_key128 = generate_random_key128();
            if(cipher_key128 == NULL) {
                log_error("Error during key generation");
                return -1;
            }
            log_info("Random key : ");
            for(int i = 0; i < 16; i++)
                printf("%02x", cipher_key128[i]);
            puts("");

            aes_128_ecb_encrypt(text_data, text_data_size64, cipher_key128, 16);
        }

        if(packer_config.debug_mode) {
            log_debug("Dumping encrypted .text ...");
            dump = fopen("text_encrypted64.dmp", "w");
            fwrite(text_data, text_data_size64, 1, dump);
            fclose(dump);
        }
    }

    return 1;
}
