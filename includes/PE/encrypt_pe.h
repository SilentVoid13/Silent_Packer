//
// Created by silentvoid on 3/19/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_ENCRYPT_PE_H
#define SILENT_CRYPT_ENCRYPT_PE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_allocation.h"

int encrypt_pe(t_pe64 *pe, char *cipher);

#endif //SILENT_CRYPT_ENCRYPT_PE_H
