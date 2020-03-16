//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_CIPHER_FUNCTIONS_H
#define SILENT_CRYPT_CIPHER_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <time.h>

extern uint64_t text_data_size;
extern uint64_t text_entry_point;
extern uint64_t cipher_key;

int generate_random_key();
uint64_t rotate_right(uint64_t value);
int xor_encrypt(char *data, size_t data_size, uint64_t key);

#endif //SILENT_CRYPT_CIPHER_FUNCTIONS_H
