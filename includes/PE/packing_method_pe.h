//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PACKING_METHOD_PE_H
#define SILENT_CRYPT_PACKING_METHOD_PE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_allocation.h"
#include "packing_method.h"

int pack_using_method_pe(t_pe64 *pe, char *packing_method);

#endif //SILENT_CRYPT_PACKING_METHOD_PE_H
