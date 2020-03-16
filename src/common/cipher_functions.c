#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "cipher_functions.h"

#include "log.h"

uint64_t text_data_size;
uint64_t text_entry_point;
uint64_t cipher_key;

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

#pragma clang diagnostic pop