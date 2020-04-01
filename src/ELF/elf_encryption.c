//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_encryption.h"
#include "cipher_functions.h"
#include "elf_allocation.h"
#include "elf_functions.h"
#include "loader_functions.h"

#include "log.h"

int encrypt_elf(t_elf *elf, char *cipher) {
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

        log_verbose("Generating random key ...");
        cipher_key32 = generate_random_key32();
        uint32_t temp_key = cipher_key32;
        log_verbose("Random key : %x", cipher_key32);

        if(strcmp(cipher, "xor") == 0) {
            xor_encrypt32(text_data, text_data_size32, temp_key);
        }
        else if(strcmp(cipher, "aes") == 0) {
            log_error("AES not implemented yet");
            return -1;
        }
    }
    else {
        text_data = ((t_elf64 *)elf)->section_data[text_section_index];

        // Setting global variables
        text_data_size64 = ((t_elf64 *)elf)->section_header[text_section_index].sh_size;
        text_entry_point64 = ((t_elf64 *)elf)->section_header[text_section_index].sh_addr;

        log_verbose("Generating random key ...");
        cipher_key64 = generate_random_key64();
        uint64_t temp_key = cipher_key64;
        log_verbose("Random key : %x", cipher_key64);

        if(strcmp(cipher, "xor") == 0) {
            xor_encrypt64(text_data, text_data_size64, temp_key);
        }
        else if(strcmp(cipher, "aes") == 0) {
            log_error("AES not implemented yet");
            return -1;
        }
    }

    return 1;
}
