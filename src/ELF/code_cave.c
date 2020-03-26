//
// Created by silentvoid on 3/7/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "code_cave.h"
#include "elf_functions.h"
#include "elf_allocation.h"
#include "packing_method_elf.h"
#include "loader_functions.h"

#include "log.h"

uint64_t loader_offset;

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
            int segment_index = find_segment_index_of_section(elf, i);
            if(elf->prog_header[segment_index].p_type == PT_LOAD) {
                //printf("i: %d\n", i);
                //printf("section_name: %s\n", get_section_name(elf, i));
                //printf("cavity_size : %d\n", code_cave_size);
                return i;
            }
        }
    }

    return -1;
}

int set_new_segment_values_cave(t_elf *elf, int segment_index) {
    elf->prog_header[segment_index].p_memsz += loader_size;
    elf->prog_header[segment_index].p_filesz += loader_size;

    add_segment_permission(elf, segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)
    add_segment_permission(elf, segment_index, PF_X); // NOLINT(hicpp-signed-bitwise)

    int text_segment_index = find_text_segment(elf);
    if(text_segment_index == -1) {
        log_error("Couldn't find .text segment");
        return -1;
    }
    add_segment_permission(elf, text_segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)

    return 1;
}

int insert_loader(t_elf *elf, int section_index, int old_section_size) {
    char *new_section_data = realloc(elf->section_data[section_index], old_section_size + loader_size);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }
    elf->section_data[section_index] = new_section_data;
    // For ASM
    loader_offset = elf->section_header[section_index].sh_addr + old_section_size;

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
    if(section_cave_index == -1) {
        log_error("Couldn't find any code cave in this ELF");
        return -1;
    }
    method_config.concerned_section = section_cave_index;

    int segment_index = find_segment_index_of_section(elf, section_cave_index);
    if(segment_index == -1) {
        log_error("Couldn't find segment index");
        return -1;
    }

    if(set_new_segment_values_cave(elf, segment_index) == -1) {
        log_error("Error during segment values modification");
        return -1;
    }

    int old_section_size = elf->section_header[section_cave_index].sh_size;
    if(insert_loader(elf, section_cave_index, old_section_size) == -1) {
        log_error("Error during Loader insertion");
        return -1;
    }

    Elf64_Addr loader_addr = elf->section_header[section_cave_index].sh_addr + old_section_size;

    printf("loader_addr : %lx\n", loader_addr);

    set_new_elf_entry_to_addr(elf, loader_addr, section_cave_index, old_section_size);

    return 1;
}
