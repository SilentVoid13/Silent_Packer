//
// Created by silentvoid on 3/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PE_SECTION_INSERTION_H
#define SILENT_CRYPT_PE_SECTION_INSERTION_H

#include "pe_allocation.h"

int add_new_pe_section_header(t_pe *pe);
int add_new_pe_section_data(t_pe *pe);
int set_new_pe_header_values(t_pe *pe);
void print_new_section_header_info(IMAGE_SECTION_HEADER *section_header);

int pe_insert_section(t_pe *pe);

#endif //SILENT_CRYPT_PE_SECTION_INSERTION_H
