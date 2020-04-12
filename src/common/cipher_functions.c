#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "cipher_functions.h"

#include "log.h"

uint32_t generate_random_key32() {
    // Note: rand() is not a really good pRNG generator but that should be ok in our case
    srand(time(NULL)); // NOLINT(cert-msc32-c,cert-msc51-cpp)
    uint32_t key = rand(); // NOLINT(cert-msc30-c,cert-msc50-cpp)
    return key;
}

uint64_t generate_random_key64() {
    // Note: rand() is not a really good pRNG generator but that should be ok in our case
    srand(time(NULL)); // NOLINT(cert-msc32-c,cert-msc51-cpp)
    uint64_t key = rand(); // NOLINT(cert-msc30-c,cert-msc50-cpp)
    return key;
}

void generate_random_key128() {
    srand(time(NULL)); // NOLINT(cert-msc32-c,cert-msc51-cpp)

    uint32_t first = rand();// NOLINT(cert-msc30-c,cert-msc50-cpp)
    uint32_t second = rand();// NOLINT(cert-msc30-c,cert-msc50-cpp)
    uint32_t third = rand();// NOLINT(cert-msc30-c,cert-msc50-cpp)
    uint32_t fourth = rand();// NOLINT(cert-msc30-c,cert-msc50-cpp)

    memcpy(cipher_key128, &first, sizeof(uint32_t));
    memcpy(cipher_key128+4, &second, sizeof(uint32_t));
    memcpy(cipher_key128+8, &third, sizeof(uint32_t));
    memcpy(cipher_key128+12, &fourth, sizeof(uint32_t));

    //uint32_t test = 0x41414141;
    //memcpy(key, &test, sizeof(uint32_t));
    //memcpy(key+4, &test, sizeof(uint32_t));
    //memcpy(key+8, &test, sizeof(uint32_t));
    //memcpy(key+12, &test, sizeof(uint32_t));
}

uint32_t rotate_right32(uint32_t value) {
    // https://www.geeksforgeeks.org/rotate-bits-of-an-integer/
    // https://blog.regehr.org/archives/1063
    uint32_t n_rotations = sizeof(uint32_t);
    uint32_t int_bits = sizeof(uint32_t) * 8;
    return (value >> n_rotations) | (value << (int_bits - n_rotations));
}

uint64_t rotate_right64(uint64_t value) {
    // https://www.geeksforgeeks.org/rotate-bits-of-an-integer/
    // https://blog.regehr.org/archives/1063
    uint64_t n_rotations = sizeof(uint64_t);
    uint64_t int_bits = sizeof(uint64_t) * 8;
    return (value >> n_rotations) | (value << (int_bits - n_rotations));
}


int xor_encrypt32(char *data, size_t data_size, uint32_t key) {
    for(int i = 0; i < (int)data_size; i++) {
        data[i] = (char)(data[i] ^ key);
        key = rotate_right32(key);
    }

    return 1;
}

int xor_encrypt64(char *data, size_t data_size, uint64_t key) {
    for(int i = 0; i < (int)data_size; i++) {
        data[i] = (char)(data[i] ^ key);
        key = rotate_right64(key);
    }

    return 1;
}

#pragma clang diagnostic pop