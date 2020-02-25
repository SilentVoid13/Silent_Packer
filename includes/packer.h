//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACKER_H
#define SILENT_CRYPT_PACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mhash.h>

int pack_file(char *file, char *cipher, char *output);

#endif //SILENT_CRYPT_PACKER_H
