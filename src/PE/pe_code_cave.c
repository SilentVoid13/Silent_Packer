//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_struct.h"
#include "pe_code_cave.h"
#include "pe_allocation.h"
#include "pe_functions.h"
#include "loader_functions.h"

#include "log.h"

int find_pe_code_cave_index(t_pe64 *pe) {
    unsigned int code_cave_size;

    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        code_cave_size = pe->pe_header->OptionalHeader.FileAlignment - pe->section_header[i].Misc.VirtualSize;
        if(code_cave_size > loader_size) {
            if(pe->section_header[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) { // NOLINT(hicpp-signed-bitwise)
                return i;
            }
        }
    }

    return -1;
}

int set_new_pe_section_values_cave(t_pe64 *pe, int section_index) {
    // TODO: Maybe add PhysicalSize ?
    pe->section_header[section_index].Misc.VirtualSize += loader_size;

    add_pe_section_permission(pe, section_index, IMAGE_SCN_MEM_WRITE); // NOLINT(hicpp-signed-bitwise)

    return 1;
}

int pe_insert_loader(t_pe64 *pe, int section_index, int old_section_size) {
    char *new_section_data = realloc(pe->section_data[section_index], old_section_size + loader_size);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }
    pe->section_data[section_index] = new_section_data;

    char *loader = patch_loader();
    if(loader == NULL) {
        log_error("Error during loader patching");
        return -1;
    }
    memcpy(new_section_data + old_section_size, loader, loader_size);

    return 1;
}

int pe_code_cave_injection(t_pe64 *pe) {
    int section_cave_index = find_pe_code_cave_index(pe);
    if(section_cave_index == -1) {
        log_error("Couldn't find any code cave in this PE file");
        return -1;
    }

    printf("Section_cave_index : %d\n", section_cave_index);

    int old_section_size = pe->section_header[section_cave_index].Misc.VirtualSize;
    if(set_new_pe_section_values_cave(pe, section_cave_index) == -1) {
        log_error("Error during section values modification");
        return -1;
    }

    if(pe_insert_loader(pe, section_cave_index, old_section_size) == -1) {
        log_error("Error during Loader insertion");
        return -1;
    }

    printf("old_section_size: %x\n", old_section_size);
    uint32_t loader_addr = pe->section_header[section_cave_index].VirtualAddress + old_section_size;
    printf("base_addr : %x\n", pe->section_header[section_cave_index].VirtualAddress);
    printf("loader_addr: %x\n", loader_addr);
    set_new_pe_entry_to_addr(pe, loader_addr, section_cave_index, old_section_size);

    return 1;
}