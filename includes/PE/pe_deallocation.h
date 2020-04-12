//
// Created by silentvoid on 4/12/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_PACKER_PE_DEALLOCATION_H
#define SILENT_PACKER_PE_DEALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pe_allocation.h"

void deallocate_pe_dos_header(t_pe *pe);
void deallocate_pe_dos_stub(t_pe *pe);
void deallocate_pe_pe_header(t_pe *pe);
void deallocate_pe_sections_headers(t_pe *pe);
void deallocate_pe_sections_data(t_pe *pe);
void deallocate_pe_sections_data_data(t_pe *pe);
void deallocate_pe_struct(t_pe *pe);
void deallocate_pe(t_pe *pe);

#endif //SILENT_PACKER_PE_DEALLOCATION_H
