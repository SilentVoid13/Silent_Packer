//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "write_elf.h"
#include "elf_allocation.h"
#include "file_functions.h"
#include "packing_method_elf.h"
#include "loader_functions.h"

#include "log.h"



int write_elf(t_elf *elf, char *filename) {
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0755); // NOLINT(hicpp-signed-bitwise)
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    // We write the ELF header
    log_verbose("Writing ELF header ...");
    write_to_file(fd, elf->elf_header, sizeof(Elf64_Ehdr));
    add_zero_padding(fd, elf->elf_header->e_phoff);

    // We write the program header
    log_verbose("Writing Program header ...");
    write_to_file(fd, elf->prog_header, sizeof(Elf64_Phdr) * elf->elf_header->e_phnum);

    // We write the section data
    log_verbose("Writing Sections data ...");
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        if(elf->section_header[i].sh_type != SHT_NOBITS) {
            add_zero_padding(fd, elf->section_header[i].sh_offset);

            // If we find the section with the code cave
            if(method_config.method_type == CODE_CAVE_METHOD && i == method_config.concerned_section) {
                write_to_file(fd, elf->section_data[i], elf->section_header[i].sh_size + loader_size);
            }
            else {
                write_to_file(fd, elf->section_data[i], elf->section_header[i].sh_size);
            }
        }
    }
    add_zero_padding(fd, elf->elf_header->e_shoff);

    // We write the section headers
    log_verbose("Writing Sections headers ...");
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        write_to_file(fd, &(elf->section_header[i]), sizeof(Elf64_Shdr));
    }

    close(fd);

    return 1;
}