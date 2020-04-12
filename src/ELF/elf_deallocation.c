//
// Created by silentvoid on 4/12/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_deallocation.h"
#include "elf_allocation.h"

void deallocate_elf_elf_header(t_elf *elf) {
    if(elf->s_type == ELF32) {
        free(((t_elf32 *)elf)->elf_header);
    }
    else {
        free(((t_elf64 *)elf)->elf_header);
    }
}

void deallocate_elf_program_header(t_elf *elf) {
    if(elf->s_type == ELF32) {
        free(((t_elf32 *)elf)->prog_header);
    }
    else {
        free(((t_elf64 *)elf)->prog_header);
    }
}

void deallocate_elf_sections_header(t_elf *elf) {
    if(elf->s_type == ELF32) {
        free(((t_elf32 *)elf)->section_header);
    }
    else {
        free(((t_elf64 *)elf)->section_header);
    }
}


void deallocate_elf_sections_data_data(t_elf *elf) {
    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            free(((t_elf32 *)elf)->section_data[i]);
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            free(((t_elf64 *)elf)->section_data[i]);
        }
    }
}

void deallocate_elf_sections_data(t_elf *elf) {
    if(elf->s_type == ELF32) {
        free(((t_elf32 *)elf)->section_data);
    }
    else {
        free(((t_elf64 *)elf)->section_data);
    }
}

void deallocate_elf_struct(t_elf *elf) {
    free(elf);
}

void deallocate_elf(t_elf *elf) {
    deallocate_elf_elf_header(elf);
    deallocate_elf_program_header(elf);
    deallocate_elf_sections_header(elf);
    deallocate_elf_sections_data_data(elf);
    deallocate_elf_sections_data(elf);
    deallocate_elf_struct(elf);
}

