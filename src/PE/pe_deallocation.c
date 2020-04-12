//
// Created by silentvoid on 4/12/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_deallocation.h"
#include "pe_allocation.h"

void deallocate_pe_dos_header(t_pe *pe) {
    if(pe->s_type == PE32) {
        free(((t_pe32 *)pe)->dos_header);
    }
    else {
        free(((t_pe64 *)pe)->dos_header);
    }
}
void deallocate_pe_dos_stub(t_pe *pe) {
    if(pe->s_type == PE32) {
        free(((t_pe32 *)pe)->dos_stub);
    }
    else {
        free(((t_pe64 *)pe)->dos_stub);
    }
}

void deallocate_pe_pe_header(t_pe *pe) {
    if(pe->s_type == PE32) {
        free(((t_pe32 *)pe)->pe_header);
    }
    else {
        free(((t_pe64 *)pe)->pe_header);
    }
}

void deallocate_pe_sections_headers(t_pe *pe) {
    if(pe->s_type == PE32) {
        free(((t_pe32 *)pe)->section_header);
    }
    else {
        free(((t_pe64 *)pe)->section_header);
    }
}

void deallocate_pe_sections_data(t_pe *pe) {
    if(pe->s_type == PE32) {
        free(((t_pe32 *)pe)->section_data);
    }
    else {
        free(((t_pe64 *)pe)->section_data);
    }
}

void deallocate_pe_sections_data_data(t_pe *pe) {
    if(pe->s_type == PE32) {
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            free(((t_pe32 *)pe)->section_data[i]);
        }
    }
    else {
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            free(((t_pe64 *)pe)->section_data[i]);
        }
    }
}

void deallocate_pe_struct(t_pe *pe) {
    free(pe);
}

void deallocate_pe(t_pe *pe) {
    deallocate_pe_dos_header(pe);
    deallocate_pe_dos_stub(pe);
    deallocate_pe_pe_header(pe);
    deallocate_pe_sections_headers(pe);
    deallocate_pe_sections_data(pe);
    deallocate_pe_sections_data_data(pe);
    deallocate_pe_struct(pe);
}
