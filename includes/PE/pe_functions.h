//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PE_FUNCTIONS_H
#define SILENT_CRYPT_PE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_allocation.h"

int set_new_pe_entry_to_section(t_pe *pe, int section_index);
int set_new_pe_entry_to_addr(t_pe *pe, uint32_t entry_addr, int section_index, int section_size);

int find_pe_text_section(t_pe *pe);
void add_pe_section_permission(t_pe *pe, int segment_index, int permission);
void print_pe_section_info(t_pe *pe, int section_index);

#endif //SILENT_CRYPT_PE_FUNCTIONS_H
