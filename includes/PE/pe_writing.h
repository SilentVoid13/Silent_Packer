//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_WRITE_PE_H
#define SILENT_PACKER_WRITE_PE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "pe_allocation.h"

int write_pe(t_pe *pe, char *filename);

#endif //SILENT_PACKER_WRITE_PE_H
