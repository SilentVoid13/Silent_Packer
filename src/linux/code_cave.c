//
// Created by silentvoid on 3/7/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "code_cave.h"
#include "elf_functions.h"
#include "elf_allocation.h"
#include "packing_method.h"

#include "log.h"

int find_cave_section_index(t_elf *elf) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        if(elf->section_header[i].sh_type == SHT_NOBITS) {
            continue;
        }

        unsigned int code_cave_size;
        // TODO: Maybe change that since it's relying on the fact that the section header is located after the section data (may not always be the case)
        if(i == elf->elf_header->e_shnum-1) {
            code_cave_size = elf->elf_header->e_shoff - (elf->section_header[i].sh_offset + elf->section_header[i].sh_size);
        }
        else {
            code_cave_size = elf->section_header[i + 1].sh_offset - (elf->section_header[i].sh_offset + elf->section_header[i].sh_size);
        }

        if(code_cave_size > loader_size) {
            return i;
        }
    }

    return 1;
}

int set_new_section_values_cave(t_elf *elf, int section_index) {
    int old_section_size = elf->section_header[section_index].sh_size;
    elf->section_header[section_index].sh_size += loader_size;

    return old_section_size;
}

int insert_loader(t_elf *elf, int section_index, int old_section_size) {
    char *new_section_data = realloc(elf->section_data[section_index], elf->section_header[section_index].sh_size);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }

    char *loader = patch_loader();
    if(loader == NULL) {
        log_error("Error during loader patching");
        return -1;
    }
    memcpy(new_section_data + old_section_size, loader, loader_size);

    return 1;
}

int code_cave_injection(t_elf *elf) {
    int section_cave_index = find_cave_section_index(elf);

    int old_section_size = set_new_section_values_cave(elf, section_cave_index);
    int segment_index = find_segment_index_of_section(elf, section_cave_index);
    if(segment_index == -1) {
        log_error("Couldn't find segment index");
        return -1;
    }
    printf("segment index : %d\n", segment_index);
    //segment_index = 2;

    add_segment_permission(elf, segment_index, PF_X); // NOLINT(hicpp-signed-bitwise)

    if(insert_loader(elf, section_cave_index, old_section_size) == -1) {
        log_error("Error during Loader insertion");
        return -1;
    }

    Elf64_Addr loader_addr = elf->section_header[section_cave_index].sh_addr + old_section_size;

    set_new_elf_entry_to_addr(elf, loader_addr, section_cave_index, old_section_size);

    return 1;
}
