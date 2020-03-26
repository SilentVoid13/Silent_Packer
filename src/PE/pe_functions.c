//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_functions.h"
#include "pe_struct.h"
#include "pe_allocation.h"
#include "loader_functions.h"

#include "log.h"

int set_new_pe_entry_to_addr(t_pe64 *pe, uint32_t entry_addr, int section_index, int section_size)  {
    uint32_t last_entry = pe->pe_header->OptionalHeader.AddressOfEntryPoint;
    pe->pe_header->OptionalHeader.AddressOfEntryPoint = entry_addr;
    int32_t jump = last_entry - (pe->pe_header->OptionalHeader.AddressOfEntryPoint + loader_size - infos_size);
    printf("jump : %d\n", jump);

    memcpy(pe->section_data[section_index] + section_size + loader_size - (infos_size + 4), &jump, 4);

    return 1;
}

int find_pe_text_section(t_pe64 *pe) {
    int index = -1;
    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        if(pe->section_header[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) { // NOLINT(hicpp-signed-bitwise)
            index = i;
        }
    }
    return index;
}

void add_pe_section_permission(t_pe64 *pe, int segment_index, int permission) {
    pe->section_header[segment_index].Characteristics |= permission; // NOLINT(hicpp-signed-bitwise)
}

void print_pe_section_info(t_pe64 *pe, int section_index) {
    puts("");
    printf("Section name : %s\n", pe->section_header[section_index].Name);
    printf("VirtualSize : %d\n", pe->section_header[section_index].Misc.VirtualSize);
    printf("SizeofRawData : %d\n", pe->section_header[section_index].SizeOfRawData);
    printf("PointerToRawData : %x\n", pe->section_header[section_index].PointerToRawData);
    printf("VirtualAddress : %x\n", pe->section_header[section_index].VirtualAddress);
}
