//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_silvio_infection.h"
#include "elf_functions.h"
#include "elf_allocation.h"
#include "loader_functions.h"
#include "elf_packing_method.h"
#include "file_functions.h"

#include "log.h"

Elf32_Addr loader_addr32;
Elf64_Addr loader_addr64;

int set_new_elf_silvio_segment_values(t_elf *elf, int text_segment_index) {
    if(elf->s_type == ELF32) {
        loader_addr32 = ((t_elf32 *)elf)->prog_header[text_segment_index].p_vaddr + ((t_elf32 *)elf)->prog_header[text_segment_index].p_filesz;

        ((t_elf32 *)elf)->prog_header[text_segment_index].p_filesz += loader_size32;
        ((t_elf32 *)elf)->prog_header[text_segment_index].p_memsz += loader_size32;

        for (int i = text_segment_index + 1; i < ((t_elf32 *)elf)->elf_header->e_phnum; i++) {
            ((t_elf32 *)elf)->prog_header[i].p_offset += PAGE_SIZE32;
        }
    }
    else {
        loader_addr64 = ((t_elf64 *)elf)->prog_header[text_segment_index].p_vaddr + ((t_elf64 *)elf)->prog_header[text_segment_index].p_filesz;

        ((t_elf64 *)elf)->prog_header[text_segment_index].p_filesz += loader_size64;
        ((t_elf64 *)elf)->prog_header[text_segment_index].p_memsz += loader_size64;

        for (int i = text_segment_index + 1; i < ((t_elf64 *)elf)->elf_header->e_phnum; i++) {
            ((t_elf64 *)elf)->prog_header[i].p_offset += PAGE_SIZE64;
        }
    }

    return 1;
}

int set_new_elf_silvio_section_values(t_elf *elf, int last_section_index) {
    if(elf->s_type == ELF32) {
        ((t_elf32 *)elf)->elf_header->e_shoff += PAGE_SIZE32;

        ((t_elf32 *)elf)->section_header[last_section_index].sh_size += loader_size32;

        for (int i = last_section_index + 1; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            ((t_elf32 *)elf)->section_header[i].sh_offset += PAGE_SIZE32;
        }
    }
    else {
        ((t_elf64 *)elf)->elf_header->e_shoff += PAGE_SIZE64;

        ((t_elf64 *)elf)->section_header[last_section_index].sh_size += loader_size64;

        for (int i = last_section_index + 1; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            ((t_elf64 *)elf)->section_header[i].sh_offset += PAGE_SIZE64;
        }
    }

    return 1;
}

int elf_silvio_insert_loader(t_elf *elf, int section_index, int old_section_size) {
    char *new_section_data;
    char *loader;
    if(elf->s_type == ELF32) {
        size_t new_section_data_size = ((t_elf32 *)elf)->section_header[section_index].sh_size;
        new_section_data = realloc(((t_elf32 *)elf)->section_data[section_index], new_section_data_size);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_elf32 *)elf)->section_data[section_index] = new_section_data;

        // For ASM
        loader_offset32 = loader_addr32;

        loader = patch_loader(x32_ARCH);
        if(loader == NULL) {
            log_error("Error during loader patching");
            return -1;
        }
        memcpy(new_section_data + old_section_size, loader, loader_size32);
    }
    else {
        size_t new_section_data_size = ((t_elf64 *)elf)->section_header[section_index].sh_size;
        new_section_data = realloc(((t_elf64 *)elf)->section_data[section_index], new_section_data_size);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_elf64 *)elf)->section_data[section_index] = new_section_data;

        // For ASM
        loader_offset64 = loader_addr64;

        loader = patch_loader(x64_ARCH);
        if(loader == NULL) {
            log_error("Error during loader patching");
            return -1;

        }
        memcpy(new_section_data + old_section_size, loader, loader_size64);
    }

    return 1;
}

int elf_silvio_infect(t_elf *elf) {
    int text_segment_index = find_elf_text_segment(elf);
    if(text_segment_index == -1) {
        log_error("Couldn't find .text segment");
        return -1;
    }


    int last_section_index = find_last_elf_section_from_segment(elf, text_segment_index);
    if(last_section_index == -1) {
        log_error("Couldn't find last section from .text segment");
        return -1;
    }
    method_config.concerned_section = last_section_index;


    int old_section_size;
    if(elf->s_type == ELF32)
        old_section_size = ((t_elf32 *)elf)->section_header[last_section_index].sh_size;
    else
        old_section_size = ((t_elf64 *)elf)->section_header[last_section_index].sh_size;


    log_verbose("Setting new section values ...");

    set_new_elf_silvio_section_values(elf, last_section_index);

    log_verbose("Setting new segment values ...");

    set_new_elf_silvio_segment_values(elf, text_segment_index);

    log_verbose("Inserting the loader ...");

    if(elf_silvio_insert_loader(elf, last_section_index, old_section_size) == -1) {
        log_error("Loader insertion failed");
        return -1;
    }

    // Add write permission to be able to decrypt it
    add_elf_segment_permission(elf, text_segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)

    log_verbose("Setting new ELF entry point ...");

    if(elf->s_type == ELF32) {
        set_new_elf_entry_to_addr32(elf, loader_addr32, last_section_index, old_section_size);
    }
    else {
        printf("loader_addr64 : %lx\n", loader_addr64);
        set_new_elf_entry_to_addr64(elf, loader_addr64, last_section_index, old_section_size);
    }

    return 1;
}