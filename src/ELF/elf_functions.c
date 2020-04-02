//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_functions.h"
#include "elf_allocation.h"
#include "loader_functions.h"
#include "all_elf_loaders_infos.h"

#include "log.h"

int set_new_elf_entry_to_section(t_elf *elf, int section_index) {
    if(elf->s_type == ELF32) {
        Elf32_Addr last_entry = ((t_elf32 *)elf)->elf_header->e_entry;
        ((t_elf32 *)elf)->elf_header->e_entry = ((t_elf32 *)elf)->section_header[section_index].sh_addr;
        int32_t jump = last_entry - (((t_elf32 *)elf)->elf_header->e_entry + I386_LINUX_ELF_LOADER_SIZE - I386_LINUX_ELF_LOADER_INFOS_SIZE);

        memcpy(((t_elf32 *)elf)->section_data[section_index] + I386_LINUX_ELF_LOADER_SIZE - (I386_LINUX_ELF_LOADER_INFOS_SIZE + 4), &jump, 4);
    }
    else {
        Elf64_Addr last_entry = ((t_elf64 *)elf)->elf_header->e_entry;
        ((t_elf64 *)elf)->elf_header->e_entry = ((t_elf64 *)elf)->section_header[section_index].sh_addr;
        int32_t jump = last_entry - (((t_elf64 *)elf)->elf_header->e_entry + AMD64_LINUX_ELF_LOADER_SIZE - AMD64_LINUX_ELF_LOADER_INFOS_SIZE);

        memcpy(((t_elf64 *)elf)->section_data[section_index] + AMD64_LINUX_ELF_LOADER_SIZE - (AMD64_LINUX_ELF_LOADER_INFOS_SIZE + 4), &jump, 4);
    }

    return 1;
}

int set_new_elf_entry_to_addr32(t_elf *elf, int32_t entry_address, int section_index, int section_size) {
    Elf32_Addr last_entry = ((t_elf32 *)elf)->elf_header->e_entry;
    ((t_elf32 *)elf)->elf_header->e_entry = entry_address;
    int32_t jump = last_entry - (((t_elf32 *)elf)->elf_header->e_entry + I386_LINUX_ELF_LOADER_SIZE - I386_LINUX_ELF_LOADER_INFOS_SIZE);

    memcpy(((t_elf32 *)elf)->section_data[section_index] + section_size + I386_LINUX_ELF_LOADER_SIZE - (I386_LINUX_ELF_LOADER_INFOS_SIZE + 4), &jump, 4);

    return 1;
}

int set_new_elf_entry_to_addr64(t_elf *elf, int64_t entry_address, int section_index, int section_size) {
    Elf64_Addr last_entry = ((t_elf64 *)elf)->elf_header->e_entry;
    ((t_elf64 *)elf)->elf_header->e_entry = entry_address;
    int32_t jump = last_entry - (((t_elf64 *)elf)->elf_header->e_entry + AMD64_LINUX_ELF_LOADER_SIZE - AMD64_LINUX_ELF_LOADER_INFOS_SIZE);

    memcpy(((t_elf64 *)elf)->section_data[section_index] + section_size + AMD64_LINUX_ELF_LOADER_SIZE - (AMD64_LINUX_ELF_LOADER_INFOS_SIZE + 4), &jump, 4);

    return 1;
}

int find_last_elf_segment_of_type(t_elf *elf, unsigned int p_type) {
    int index = -1;

    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf32 *)elf)->prog_header[i].p_type == p_type) {
                index = i;
            }
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf64 *)elf)->prog_header[i].p_type == p_type) {
                index = i;
            }
        }
    }

    return index;
}

// Find last section from specified segment index
int find_last_elf_section_from_segment(t_elf *elf, int segment_index) {
    int index = -1;

    if(elf->s_type == ELF32) {
        Elf32_Phdr *program_header = ((t_elf32 *)elf)->prog_header + segment_index;
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            Elf32_Shdr *section_header = ((t_elf32 *)elf)->section_header + i;

            if (section_header->sh_addr >= program_header->p_vaddr
                && section_header->sh_addr < program_header->p_vaddr + program_header->p_memsz
                    ) {
                index = i;
            }
        }
    }
    else {
        Elf64_Phdr *program_header = ((t_elf64 *)elf)->prog_header + segment_index;
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            Elf64_Shdr *section_header = ((t_elf64 *)elf)->section_header + i;

            if (section_header->sh_addr >= program_header->p_vaddr
                && section_header->sh_addr < program_header->p_vaddr + program_header->p_memsz
                    ) {
                index = i;
            }
        }
    }

    return index;
}

// Find the text segment based on the fact that the elf entry point is located at the start of the .text section
int find_elf_text_segment(t_elf *elf) {
    int index = -1;

    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf32 *)elf)->prog_header[i].p_type == PT_LOAD
                && ((t_elf32 *)elf)->elf_header->e_entry < (((t_elf32 *)elf)->prog_header[i].p_vaddr + ((t_elf32 *)elf)->prog_header[i].p_filesz)
                && ((t_elf32 *)elf)->elf_header->e_entry > ((t_elf32 *)elf)->prog_header[i].p_vaddr) {
                index = i;
                break;
            }
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf64 *)elf)->prog_header[i].p_type == PT_LOAD
                && ((t_elf64 *)elf)->elf_header->e_entry < (((t_elf64 *)elf)->prog_header[i].p_vaddr + ((t_elf64 *)elf)->prog_header[i].p_filesz)
                && ((t_elf64 *)elf)->elf_header->e_entry > ((t_elf64 *)elf)->prog_header[i].p_vaddr) {
                index = i;
                break;
            }
        }
    }

    return index;
}

int find_elf_segment_index_of_section(t_elf *elf, int section_index) {
    int index = -1;

    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf32 *)elf)->prog_header[i].p_offset <= ((t_elf32 *)elf)->section_header[section_index].sh_offset) {
                index = i;
            } else {
                if (index == -1)
                    index = 0;
                break;
            }
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_phnum; i++) {
            if (((t_elf64 *)elf)->prog_header[i].p_offset <= ((t_elf64 *)elf)->section_header[section_index].sh_offset) {
                index = i;
            } else {
                if (index == -1)
                    index = 0;
                break;
            }
        }
    }

    return index;
}

char* find_elf_section_name(t_elf *elf, int index) {
    // sh_name contains the index into the section string table of the section name string
    if(elf->s_type == ELF32) {
        int section_string_table_index = ((t_elf32 *)elf)->elf_header->e_shstrndx;
        return ((char *) (((t_elf32 *)elf)->section_data[section_string_table_index] + ((t_elf32 *)elf)->section_header[index].sh_name));
    }
    else {
        int section_string_table_index = ((t_elf64 *)elf)->elf_header->e_shstrndx;
        return ((char *) (((t_elf64 *)elf)->section_data[section_string_table_index] + ((t_elf64 *)elf)->section_header[index].sh_name));
    }
}

int find_elf_section_index(t_elf *elf, char *section_name) {
    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            char *s_name = find_elf_section_name(elf, i);
            if (strcmp(section_name, s_name) == 0) {
                return i;
            }
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            char *s_name = find_elf_section_name(elf, i);
            if (strcmp(section_name, s_name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

void add_elf_segment_permission(t_elf *elf, int segment_index, int permission) {
    if(elf->s_type == ELF32)
        ((t_elf32 *)elf)->prog_header[segment_index].p_flags |= permission; // NOLINT(hicpp-signed-bitwise)
    else
        ((t_elf64 *)elf)->prog_header[segment_index].p_flags |= permission; // NOLINT(hicpp-signed-bitwise)
}
