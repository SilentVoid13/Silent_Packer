//
// Created by silentvoid on 3/18/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_struct.h"
#include "pe_code_cave.h"
#include "pe_allocation.h"
#include "pe_functions.h"
#include "loader_functions.h"
#include "file_functions.h"
#include "all_pe_loaders_infos.h"

#include "log.h"

int find_pe_code_cave_index(t_pe *pe) {
    size_t code_cave_size;

    if(pe->s_type == PE32) {
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections - 1; i++) {
            if (((t_pe32 *)pe)->section_header[i].SizeOfRawData > ((t_pe32 *)pe)->section_header[i].Misc.VirtualSize) {
                code_cave_size = ((t_pe32 *)pe)->section_header[i].SizeOfRawData - ((t_pe32 *)pe)->section_header[i].Misc.VirtualSize;

                //print_pe_section_info(pe, i);

                if (code_cave_size > I386_WIN_PE_LOADER_SIZE) {
                    return i;
                }
            }
        }
    }
    else {
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections - 1; i++) {
            if (((t_pe64 *)pe)->section_header[i].SizeOfRawData > ((t_pe64 *)pe)->section_header[i].Misc.VirtualSize) {
                code_cave_size = ((t_pe64 *)pe)->section_header[i].SizeOfRawData - ((t_pe64 *)pe)->section_header[i].Misc.VirtualSize;

                //print_pe_section_info(pe, i);

                if (code_cave_size > AMD64_WIN_PE_LOADER_SIZE) {
                    return i;
                }
            }
        }
    }

    return -1;
}

int set_new_pe_cave_section_values(t_pe *pe, int section_index) {
    if(pe->s_type == PE32)
        ((t_pe32 *)pe)->section_header[section_index].Misc.VirtualSize += I386_WIN_PE_LOADER_SIZE;
    else
        ((t_pe64 *)pe)->section_header[section_index].Misc.VirtualSize += AMD64_WIN_PE_LOADER_SIZE;

    // TODO: Change this to use mprotect in the loader

    int text_section_index = find_pe_text_section(pe);
    if(text_section_index == -1) {
        log_error("Couldn't find .text segment");
        return -1;
    }
    add_pe_section_permission(pe, text_section_index, IMAGE_SCN_MEM_WRITE); // NOLINT(hicpp-signed-bitwise)

    add_pe_section_permission(pe, section_index, IMAGE_SCN_MEM_WRITE); // NOLINT(hicpp-signed-bitwise)
    add_pe_section_permission(pe, section_index, IMAGE_SCN_MEM_READ); // NOLINT(hicpp-signed-bitwise)
    add_pe_section_permission(pe, section_index, IMAGE_SCN_MEM_EXECUTE); // NOLINT(hicpp-signed-bitwise)

    return 1;
}

int pe_cave_insert_loader(t_pe *pe, int section_index, int old_section_size) {
    char *new_section_data;

    if(pe->s_type == PE32) {
        new_section_data = realloc(((t_pe32 *)pe)->section_data[section_index], old_section_size + I386_WIN_PE_LOADER_SIZE);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_pe32 *)pe)->section_data[section_index] = new_section_data;

        // For ASM
        loader_offset32 = ((t_pe32 *)pe)->section_header[section_index].VirtualAddress + old_section_size;

        char *loader = patch_loader(x32_ARCH, PE32);
        if(loader == NULL) {
            log_error("Error during loader patching");
            return -1;
        }
        memcpy(new_section_data + old_section_size, loader, I386_WIN_PE_LOADER_SIZE);
    }
    else {
        new_section_data = realloc(((t_pe64 *)pe)->section_data[section_index], old_section_size + AMD64_WIN_PE_LOADER_SIZE);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_pe64 *)pe)->section_data[section_index] = new_section_data;

        // For ASM
        loader_offset64 = ((t_pe64 *)pe)->section_header[section_index].VirtualAddress + old_section_size;

        char *loader = patch_loader(x64_ARCH, PE64);
        if(loader == NULL) {
            log_error("Error during loader patching");
            return -1;
        }
        memcpy(new_section_data + old_section_size, loader, AMD64_WIN_PE_LOADER_SIZE);
    }

    return 1;
}

int pe_code_cave_injection(t_pe *pe) {

    log_verbose("Finding a code cave ...");

    int section_cave_index = find_pe_code_cave_index(pe);
    if(section_cave_index == -1) {
        log_error("Couldn't find any code cave in this PE file");
        return -1;
    }

    log_verbose("Setting new section values ...");


    int old_section_size;
    if(pe->s_type == PE32)
        old_section_size = ((t_pe32 *)pe)->section_header[section_cave_index].Misc.VirtualSize;
    else
        old_section_size = ((t_pe64 *)pe)->section_header[section_cave_index].Misc.VirtualSize;

    if(set_new_pe_cave_section_values(pe, section_cave_index) == -1) {
        log_error("Error during section values modification");
        return -1;
    }

    log_verbose("Inserting the loader inside the code cave ...");

    if(pe_cave_insert_loader(pe, section_cave_index, old_section_size) == -1) {
        log_error("Error during Loader insertion");
        return -1;
    }

    log_verbose("Setting new PE entry point ...");

    uint32_t loader_addr;
    if(pe->s_type == PE32)
        loader_addr = ((t_pe32 *)pe)->section_header[section_cave_index].VirtualAddress + old_section_size;
    else
        loader_addr = ((t_pe64 *)pe)->section_header[section_cave_index].VirtualAddress + old_section_size;

    set_new_pe_entry_to_addr(pe, loader_addr, section_cave_index, old_section_size);

    return 1;
}