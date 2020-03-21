//
// Created by silentvoid on 3/19/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "encrypt_pe.h"
#include "cipher_functions.h"
#include "pe_allocation.h"
#include "pe_functions.h"

#include "log.h"

int encrypt_pe(t_pe64 *pe, char *cipher) {
    int text_section_index = find_pe_text_section(pe);
    if(text_section_index == -1) {
        log_error("Couldn't find .text section");
        return -1;
    }

    log_verbose("Got .text section index : %d", text_section_index);

    // Setting global variables
    text_data_size = pe->section_header[text_section_index].SizeOfRawData;
    text_entry_point = pe->section_header[text_section_index].PointerToRawData;

    log_verbose("Generating random key ...");
    cipher_key = generate_random_key();
    uint64_t temp_key = cipher_key;
    log_verbose("Random key : %d", cipher_key);

    char *text_data = pe->section_data[text_section_index];

    if(strcmp(cipher, "xor") == 0) {
        xor_encrypt(text_data, text_data_size, temp_key);
    }
    else if(strcmp(cipher, "aes") == 0) {

    }

    return 1;
}
