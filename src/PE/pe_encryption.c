//
// Created by silentvoid on 3/19/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_encryption.h"
#include "pe_struct.h"
#include "cipher_functions.h"
#include "pe_allocation.h"
#include "pe_functions.h"

#include "log.h"

int encrypt_pe(t_pe *pe, char *cipher) {
    int text_section_index = find_pe_text_section(pe);
    if(text_section_index == -1) {
        log_error("Couldn't find .text section");
        return -1;
    }

    log_verbose("Got .text section index : %d", text_section_index);


    log_verbose("Generating random key ...");
    cipher_key = generate_random_key();
    uint64_t temp_key = cipher_key;
    log_verbose("Random key : %d", cipher_key);

    char *text_data;

    if(pe->s_type == PE32) {
        // Setting global variables
        text_entry_point = ((t_pe32 *)pe)->section_header[text_section_index].VirtualAddress;
        text_data_size = ((t_pe32 *)pe)->section_header[text_section_index].Misc.VirtualSize;

        text_data = ((t_pe32 *)pe)->section_data[text_section_index];
    }
    else {
        // Setting global variables
        text_entry_point = ((t_pe64 *)pe)->section_header[text_section_index].VirtualAddress;
        text_data_size = ((t_pe64 *)pe)->section_header[text_section_index].Misc.VirtualSize;

        text_data = ((t_pe64 *)pe)->section_data[text_section_index];
    }

    if(strcmp(cipher, "xor") == 0) {
        xor_encrypt(text_data, text_data_size, temp_key);
    }
    else if(strcmp(cipher, "aes") == 0) {

    }

    return 1;
}
