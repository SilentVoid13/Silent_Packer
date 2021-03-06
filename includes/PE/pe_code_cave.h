//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_CODE_CAVE_H
#define SILENT_PACKER_CODE_CAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_allocation.h"

int find_pe_code_cave_index(t_pe *pe);
int set_new_pe_cave_section_values(t_pe *pe, int section_index);
int pe_cave_insert_loader(t_pe *pe, int section_index, int old_section_size);

int pe_code_cave_injection(t_pe *pe);

#endif //SILENT_PACKER_CODE_CAVE_H
