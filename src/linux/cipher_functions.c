#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "cipher_functions.h"
#include "elf_allocation.h"

#include "log.h"

uint64_t text_data_size;
uint64_t text_entry_point;
uint64_t cipher_key;

char* get_section_name(t_elf *elf, int index) {
    int section_string_table_index = elf->elf_header->e_shstrndx;
    return ((char *)(elf->section_data[section_string_table_index] + elf->section_header[index].sh_name));
}

int get_section_index(t_elf *elf, char *section_name) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        char *s_name = get_section_name(elf, i);
        if(strcmp(section_name, s_name) == 0) {
            return i;
        }
    }
    return -1;
}

int generate_random_key() {
    // Note: rand() is not a really good pRNG generator but that should be ok in our case
    srand(time(NULL)); // NOLINT(cert-msc32-c,cert-msc51-cpp)
    int key = rand(); // NOLINT(cert-msc30-c,cert-msc50-cpp)
    return key;
}

uint64_t rotate_right(uint64_t value) {
    // https://www.geeksforgeeks.org/rotate-bits-of-an-integer/
    // https://blog.regehr.org/archives/1063
    uint64_t n_rotations = 8;
    uint64_t int_bits = 64;
    return (value >> n_rotations) | (value << (int_bits - n_rotations));
}

int xor_encrypt(char *data, size_t data_size, uint64_t key) {
    for(int i = 0; i < (int)data_size; i++) {
        data[i] = (char)(data[i] ^ key);
        key = rotate_right(key);
    }

    return 1;
}

int encrypt_data(t_elf *elf, char *cipher) {
    int text_section_index = get_section_index(elf, ".text");
    if(text_section_index == -1) {
        log_error("Couldn't find .text section in the ELF binary");
        return -1;
    }

    log_verbose("Got .text section index : %d", text_section_index);

    char *text_data = elf->section_data[text_section_index];

    // Setting global variables
    text_data_size = elf->section_header[text_section_index].sh_size;
    text_entry_point = elf->section_header[text_section_index].sh_addr;

    log_verbose("Generating random key ...");
    cipher_key = generate_random_key();
    uint64_t temp_key = cipher_key;

    if(strcmp(cipher, "xor") == 0) {
        xor_encrypt(text_data, text_data_size, temp_key);
    }

    return 1;
}

#pragma clang diagnostic pop