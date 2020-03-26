//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACKING_METHOD_H
#define SILENT_CRYPT_PACKING_METHOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTION_INSERTION_METHOD 1
#define SILVIO_INFECTION_METHOD 2
#define CODE_CAVE_METHOD 3

struct method_config {
    int method_type;
    int concerned_section;
};

extern struct method_config method_config;

#endif //SILENT_CRYPT_PACKING_METHOD_H
