//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_silvio_infection.h"
#include "elf_functions.h"
#include "elf_allocation.h"
#include "loader_functions.h"
#include "elf_packing_method.h"

#include "log.h"

Elf64_Addr loader_addr;
int old_section_size;

int set_new_elf_silvio_segment_values(t_elf *elf, int text_segment_index) {
    loader_addr = elf->prog_header[text_segment_index].p_vaddr + elf->prog_header[text_segment_index].p_filesz;

    elf->prog_header[text_segment_index].p_filesz += loader_size;
    elf->prog_header[text_segment_index].p_memsz += loader_size;

    for(int i = text_segment_index + 1; i < elf->elf_header->e_phnum; i++) {
        elf->prog_header[i].p_offset += PAGE_SIZE64;
    }

    return 1;
}

int set_new_elf_silvio_section_values(t_elf *elf, int last_section_index) {
    old_section_size = elf->section_header[last_section_index].sh_size;
    elf->section_header[last_section_index].sh_size += loader_size;

    for(int i = last_section_index + 1; i < elf->elf_header->e_shnum; i++) {
        elf->section_header[i].sh_offset += PAGE_SIZE64;
    }

    return 1;
}

int elf_silvio_insert_loader(t_elf *elf, int section_index) {
    size_t new_section_data_size = elf->section_header[section_index].sh_size;
    char *new_section_data = realloc(elf->section_data[section_index], new_section_data_size);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }
    elf->section_data[section_index] = new_section_data;

    // For ASM
    loader_offset = loader_addr;

    char *loader = patch_loader();
    if(loader == NULL) {
        log_error("Error during loader patching");
        return -1;
    }
    memcpy(new_section_data + old_section_size, loader, loader_size);

    return 1;
}

int elf_silvio_infect(t_elf *elf) {
    int text_segment_index = find_elf_text_segment(elf);
    if(text_segment_index == -1) {
        log_error("Couldn't find .text segment");
        return -1;
    }

    log_verbose("Setting new segment values ...");

    set_new_elf_silvio_segment_values(elf, text_segment_index);

    int last_section_index = find_last_elf_section_from_segment(elf, text_segment_index);
    if(last_section_index == -1) {
        log_error("Couldn't find last section from .text segment");
        return -1;
    }
    method_config.concerned_section = last_section_index;

    elf->elf_header->e_shoff += PAGE_SIZE64;
    set_new_elf_silvio_section_values(elf, last_section_index);

    log_verbose("Inserting the loader ...");

    if(elf_silvio_insert_loader(elf, last_section_index) == -1) {
        log_error("Loader insertion failed");
        return -1;
    }

    // Add write permission to be able to decrypt it
    add_elf_segment_permission(elf, text_segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)

    log_verbose("Setting new ELF entry point ...");

    // Set jump address + new elf entry point
    set_new_elf_entry_to_addr(elf, loader_addr, last_section_index, old_section_size);

    return 1;
}