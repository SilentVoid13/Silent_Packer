//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_functions.h"
#include "pe_struct.h"
#include "pe_allocation.h"
#include "loader_functions.h"

#include "log.h"

int set_new_pe_entry_to_addr(t_pe *pe, uint32_t entry_addr, int section_index, int section_size)  {
    if(pe->s_type == PE32) {
        uint32_t last_entry = ((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint;
        ((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint = entry_addr;
        int32_t jump = last_entry - (((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint + loader_size32 - infos_size32);

        memcpy(((t_pe32 *)pe)->section_data[section_index] + section_size + loader_size32 - (infos_size32 + 4), &jump, 4);
    }
    else {
        uint32_t last_entry = ((t_pe64 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint;
        ((t_pe64 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint = entry_addr;
        int32_t jump = last_entry - (((t_pe64 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint + loader_size64 - infos_size64);

        memcpy(((t_pe64 *)pe)->section_data[section_index] + section_size + loader_size64 - (infos_size64 + 4), &jump, 4);
    }

    return 1;
}

int set_new_pe_entry_to_section(t_pe *pe, int section_index) {
    if(pe->s_type == PE32) {
        uint32_t last_entry = ((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint;
        ((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint = ((t_pe32 *)pe)->section_header[section_index].VirtualAddress;
        int32_t jump = last_entry - (((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint + loader_size32 - infos_size32);

        memcpy(((t_pe32 *)pe)->section_data[section_index] + loader_size32 - (infos_size32 + 4), &jump, 4);
    }
    else {
        uint32_t last_entry = ((t_pe64 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint;
        ((t_pe32 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint = ((t_pe64 *)pe)->section_header[section_index].VirtualAddress;
        int32_t jump = last_entry - (((t_pe64 *)pe)->pe_header->OptionalHeader.AddressOfEntryPoint + loader_size64 - infos_size64);

        memcpy(((t_pe64 *)pe)->section_data[section_index] + loader_size64 - (infos_size64 + 4), &jump, 4);
    }

    return 1;
}

int find_pe_text_section(t_pe *pe) {
    int index = -1;
    if(pe->s_type == PE32) {
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (((t_pe32 *)pe)->section_header[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) { // NOLINT(hicpp-signed-bitwise)
                index = i;
            }
        }
    }
    else {
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (((t_pe64 *)pe)->section_header[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) { // NOLINT(hicpp-signed-bitwise)
                index = i;
            }
        }
    }
    return index;
}

void add_pe_section_permission(t_pe *pe, int segment_index, int permission) {
    if(pe->s_type == PE32) {
        ((t_pe32 *)pe)->section_header[segment_index].Characteristics |= permission; // NOLINT(hicpp-signed-bitwise)
    }
    else {
        ((t_pe64 *)pe)->section_header[segment_index].Characteristics |= permission; // NOLINT(hicpp-signed-bitwise)
    }
}

void print_pe_section_info(t_pe *pe, int section_index) {
    puts("");

    if(pe->s_type == PE32) {
        printf("Section name : %s\n", ((t_pe32 *)pe)->section_header[section_index].Name);
        printf("VirtualSize : %d\n", ((t_pe32 *)pe)->section_header[section_index].Misc.VirtualSize);
        printf("SizeofRawData : %d\n", ((t_pe32 *)pe)->section_header[section_index].SizeOfRawData);
        printf("PointerToRawData : %x\n", ((t_pe32 *)pe)->section_header[section_index].PointerToRawData);
        printf("VirtualAddress : %x\n", ((t_pe32 *)pe)->section_header[section_index].VirtualAddress);
    }
    else {
        printf("Section name : %s\n", ((t_pe64 *)pe)->section_header[section_index].Name);
        printf("VirtualSize : %d\n", ((t_pe64 *)pe)->section_header[section_index].Misc.VirtualSize);
        printf("SizeofRawData : %d\n", ((t_pe64 *)pe)->section_header[section_index].SizeOfRawData);
        printf("PointerToRawData : %x\n", ((t_pe64 *)pe)->section_header[section_index].PointerToRawData);
        printf("VirtualAddress : %x\n", ((t_pe64 *)pe)->section_header[section_index].VirtualAddress);
    }
}
