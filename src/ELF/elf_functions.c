//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_functions.h"
#include "elf_allocation.h"
#include "loader_functions.h"

#include "log.h"

int set_new_elf_entry_to_section(t_elf *elf, int section_index) {
    Elf64_Addr last_entry = elf->elf_header->e_entry;
    elf->elf_header->e_entry = elf->section_header[section_index].sh_addr;
    int32_t jump = last_entry - (elf->elf_header->e_entry + loader_size - infos_size);

    memcpy(elf->section_data[section_index] + loader_size - (infos_size + 4), &jump, 4);

    return 1;
}

int set_new_elf_entry_to_addr(t_elf *elf, int64_t entry_address, int section_index, int section_size) {
    Elf64_Addr last_entry = elf->elf_header->e_entry;
    elf->elf_header->e_entry = entry_address;
    int32_t jump = last_entry - (elf->elf_header->e_entry + loader_size - infos_size);

    memcpy(elf->section_data[section_index] + section_size + loader_size - (infos_size + 4), &jump, 4);

    return 1;
}

int find_last_segment_of_type(t_elf *elf, unsigned int p_type) {
    int index = -1;
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        if(elf->prog_header[i].p_type == p_type) {
            index = i;
        }
    }
    return index;
}

void add_segment_permission(t_elf *elf, int segment_index, int permission) {
    elf->prog_header[segment_index].p_flags |= permission; // NOLINT(hicpp-signed-bitwise)
}

// Find last loadable section
int find_last_section(t_elf *elf, int segment_index) {
    int index = -1;
    Elf64_Phdr *program_header = elf->prog_header + segment_index;
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        Elf64_Shdr	*section_header = elf->section_header + i;
        if(section_header->sh_addr + section_header->sh_size >= program_header->p_vaddr + program_header->p_memsz) {
            index = i;
        }
    }
    return index;
}

// Find last section from specified segment index
int find_last_section_from_segment(t_elf *elf, int segment_index) {
    int index = -1;
    Elf64_Phdr *program_header = elf->prog_header + segment_index;

    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        Elf64_Shdr	*section_header = elf->section_header + i;

        if(section_header->sh_addr >= program_header->p_vaddr
        && section_header->sh_addr < program_header->p_vaddr + program_header->p_memsz
        ) {
            index = i;
        }
    }
    return index;
}

// Find the text segment based on the fact that the elf entry point is located at the start of the .text section
int find_text_segment(t_elf *elf) {
    int index = -1;
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        if (elf->prog_header[i].p_type == PT_LOAD
        && elf->elf_header->e_entry < (elf->prog_header[i].p_vaddr + elf->prog_header[i].p_filesz)
        && elf->elf_header->e_entry > elf->prog_header[i].p_vaddr) {
            index = i;
            break;
        }
    }
    return index;
}

int find_segment_index_of_section(t_elf *elf, int section_index) {
    int index = -1;
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        //printf("prog_header v_addr : %ld\n", elf->prog_header[i].p_offset);
        //printf("section_addr : %ld\n", elf->section_header[section_index].sh_offset);
        if(elf->prog_header[i].p_offset <= elf->section_header[section_index].sh_offset) {
            index = i;
        }
        else {
            if(index == -1)
                index = 0;
            break;
        }
    }

    return index;
}

char* get_section_name(t_elf *elf, int index) {
    int section_string_table_index = elf->elf_header->e_shstrndx;
    // sh_name contains the index into the section string table of the section name string
    return ((char *)(elf->section_data[section_string_table_index] + elf->section_header[index].sh_name));
}

int get_section_index(t_elf *elf, char *section_name) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        char *s_name = get_section_name(elf, i);
        if(strcmp(section_name, s_name) == 0) {
            return i;
        }
    }
    return -1;
}
