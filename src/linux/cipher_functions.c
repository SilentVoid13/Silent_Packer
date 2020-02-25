#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "cipher_functions.h"
#include "elf_allocation.h"

#include "log.h"

char* get_section_name(t_elf *elf, int index) {
    int section_string_table_entry = elf->elf_header->e_shstrndx;
    printf("sh_name : %d\n", elf->section_header[index].sh_name);
    return ((char *)(elf->section_data[section_string_table_entry] + elf->section_header[index].sh_name));
}

int get_section_index(t_elf *elf, char *section_name) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        char *s_name = get_section_name(elf, i);
        printf("s_name : %s\n", s_name);
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


int rotate_right(uint64_t value) {
    // https://www.geeksforgeeks.org/rotate-bits-of-an-integer/
    // https://blog.regehr.org/archives/1063
    int n_rotations = 8;
    int int_bits = 64;
    return (value >> n_rotations) | (value << (int_bits - n_rotations));
}

int xor_encrypt(int *data, size_t data_size, int key) {
    for(int i = 0; i < (int)data_size; i++) {
        data[i] ^= key;
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

    size_t text_data_size = elf->section_header[text_section_index].sh_size;
    int *text_data = elf->section_data[text_section_index];

    //int entry_point_address = elf->section_header[text_section_index].sh_addr;
    //size_t entry_point_size = text_data_size;

    int random_key = generate_random_key();

    if(strcmp(cipher, "xor") == 0) {
        xor_encrypt(text_data, text_data_size, random_key);
    }

    return 1;
}

#pragma clang diagnostic pop