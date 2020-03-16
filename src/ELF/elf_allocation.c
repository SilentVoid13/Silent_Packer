//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "ELF/elf_allocation.h"

#include "log.h"

int allocate_elf_elf_header(t_elf *elf, void *file_data, size_t file_data_size) {
    // http://www.openvirtualization.org/documentation/structElf64__Ehdr.html

    if(file_data_size < sizeof(Elf64_Ehdr)) {
        log_error("Total file size is less than ELF Header size");
        return -1;
    }

    elf->elf_header = malloc(sizeof(Elf64_Ehdr));
    if(elf->elf_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(elf->elf_header, file_data, sizeof(Elf64_Ehdr));

    // https://code.woboq.org/userspace/glibc/elf/elf.h.html
    // "\177ELF"
    if(strncmp((char *)elf->elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        log_error("Magic bytes does not match ELF bytes");
        return -1;
    }

    if(elf->elf_header->e_machine != EM_X86_64) {
        log_error("Currently only x86_64 ELF binaries are supported");
        return -1;
    }

    return 1;
}

int allocate_elf_program_header(t_elf *elf, void *file_data, size_t file_data_size) {
    size_t elf_program_header_size = sizeof(Elf64_Phdr) * elf->elf_header->e_phnum;
    if(file_data_size < sizeof(Elf64_Ehdr) + elf_program_header_size) {
        log_error("Total file size is inferior to ELF Header + Program Header size");
        return -1;
    }

    elf->prog_header = malloc(elf_program_header_size);
    if(elf->prog_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(elf->prog_header, file_data + elf->elf_header->e_phoff, elf_program_header_size);

    return 1;
}

int allocate_elf_sections_header(t_elf *elf, void *file_data, size_t file_data_size) {
    size_t elf_sections_header_size = sizeof(Elf64_Shdr) * elf->elf_header->e_shnum;

    elf->section_header = malloc(elf_sections_header_size);
    if(elf->section_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(elf->section_header, 0, elf_sections_header_size);

    // We copy each section header to our struct
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        if(file_data_size < elf->elf_header->e_shoff + (i * sizeof(Elf64_Shdr))) {
            log_error("Total file size is inferior to ELF section header size");
            return -1;
        }
        memcpy(&(elf->section_header[i]), file_data + elf->elf_header->e_shoff + (i * sizeof(Elf64_Shdr)), sizeof(Elf64_Shdr));
    }

    return 1;
}

int allocate_elf_sections_data(t_elf *elf, void *file_data, size_t file_data_size) {
    elf->section_data = malloc(sizeof(char *) * elf->elf_header->e_shnum);
    if(elf->section_data == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(elf->section_data, 0, sizeof(char *) * elf->elf_header->e_shnum);

    size_t elf_section_data_size;
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        // Section that occupies no space in file
        if(elf->section_header[i].sh_type == SHT_NOBITS) {
            elf->section_data[i] = NULL;
        }
        else {
            if(file_data_size < elf->section_header[i].sh_offset) {
                // TODO: Change message
                log_error("Total file size is less than section data offset");
                return -1;
            }

            elf_section_data_size = elf->section_header[i].sh_size;

            elf->section_data[i] = malloc(elf_section_data_size);
            if(elf->section_data[i] == NULL) {
                log_error("malloc() failure");
                return -1;
            }
            memset(elf->section_data[i], 0, elf_section_data_size);
            memcpy(elf->section_data[i], file_data + elf->section_header[i].sh_offset, elf_section_data_size);
        }
    }

    return 1;
}

int allocate_elf(t_elf **elf, void *file_data, size_t file_data_size) {
    size_t t_elf_size = sizeof(t_elf);

    *elf = malloc(t_elf_size);
    if(*elf == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(*elf, 0, t_elf_size);

    if(allocate_elf_elf_header(*elf, file_data, file_data_size) == -1) {
        log_error("Error during ELF Header allocation");
        return -1;
    }

    if(allocate_elf_program_header(*elf, file_data, file_data_size) == -1) {
        log_error("Error during Program Header allocation");
        return -1;
    }

    if(allocate_elf_sections_header(*elf, file_data, file_data_size) == -1) {
        log_error("Error during Section Header allocation");
        return -1;
    }

    if(allocate_elf_sections_data(*elf, file_data, file_data_size) == -1) {
        log_error("Error during Section Data allocation");
        return -1;
    }

    return 1;
}
