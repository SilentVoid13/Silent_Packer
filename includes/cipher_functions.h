//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_CIPHER_FUNCTIONS_H
#define SILENT_PACKER_CIPHER_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <time.h>

extern uint32_t text_data_size32;
extern uint32_t text_entry_point32;
extern uint32_t cipher_key32;

extern uint64_t text_data_size64;
extern uint64_t text_entry_point64;
extern uint64_t cipher_key64;

uint32_t generate_random_key32();
uint64_t generate_random_key64();

uint32_t rotate_right32(uint32_t value);
uint64_t rotate_right64(uint64_t value);

int xor_encrypt32(char *data, size_t data_size, uint32_t key);
int xor_encrypt64(char *data, size_t data_size, uint64_t key);

#endif //SILENT_PACKER_CIPHER_FUNCTIONS_H
