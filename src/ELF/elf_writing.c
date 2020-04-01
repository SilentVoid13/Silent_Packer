//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_writing.h"
#include "elf_allocation.h"
#include "file_functions.h"
#include "elf_packing_method.h"
#include "loader_functions.h"

#include "log.h"



int write_elf(t_elf *elf, char *filename) {
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0755); // NOLINT(hicpp-signed-bitwise)
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    if(elf->s_type == ELF32) {
        log_verbose("Writing ELF header ...");
        write_to_file(fd, ((t_elf32 *)elf)->elf_header, sizeof(Elf32_Ehdr));
        add_zero_padding(fd, ((t_elf32 *)elf)->elf_header->e_phoff);

        log_verbose("Writing Program header ...");
        write_to_file(fd, ((t_elf32 *)elf)->prog_header, sizeof(Elf32_Phdr) * ((t_elf32 *)elf)->elf_header->e_phnum);

        log_verbose("Writing Sections data ...");
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            if (((t_elf32 *)elf)->section_header[i].sh_type != SHT_NOBITS) {
                add_zero_padding(fd, ((t_elf32 *)elf)->section_header[i].sh_offset);

                // If we find the section with the code cave
                if (method_config.method_type == CODE_CAVE_METHOD && i == method_config.concerned_section) {
                    write_to_file(fd, ((t_elf32 *)elf)->section_data[i], ((t_elf32 *)elf)->section_header[i].sh_size + loader_size32);
                } else {
                    write_to_file(fd, ((t_elf32 *)elf)->section_data[i], ((t_elf32 *)elf)->section_header[i].sh_size);
                }
            }
        }
        add_zero_padding(fd, ((t_elf32 *)elf)->elf_header->e_shoff);

        log_verbose("Writing Sections headers ...");
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            write_to_file(fd, &(((t_elf32 *)elf)->section_header[i]), sizeof(Elf32_Shdr));
        }
    }
    else {
        log_verbose("Writing ELF header ...");
        write_to_file(fd, ((t_elf64 *)elf)->elf_header, sizeof(Elf64_Ehdr));
        add_zero_padding(fd, ((t_elf64 *)elf)->elf_header->e_phoff);

        log_verbose("Writing Program header ...");
        write_to_file(fd, ((t_elf64 *)elf)->prog_header, sizeof(Elf64_Phdr) * ((t_elf64 *)elf)->elf_header->e_phnum);

        log_verbose("Writing Sections data ...");
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            if (((t_elf64 *)elf)->section_header[i].sh_type != SHT_NOBITS) {
                add_zero_padding(fd, ((t_elf64 *)elf)->section_header[i].sh_offset);

                // If we find the section with the code cave
                if (method_config.method_type == CODE_CAVE_METHOD && i == method_config.concerned_section) {
                    write_to_file(fd, ((t_elf64 *)elf)->section_data[i], ((t_elf64 *)elf)->section_header[i].sh_size + loader_size64);
                } else {
                    write_to_file(fd, ((t_elf64 *)elf)->section_data[i], ((t_elf64 *)elf)->section_header[i].sh_size);
                }
            }
        }
        add_zero_padding(fd, ((t_elf64 *)elf)->elf_header->e_shoff);

        log_verbose("Writing Sections headers ...");
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            write_to_file(fd, &(((t_elf64 *) elf)->section_header[i]), sizeof(Elf64_Shdr));
        }
    }

    close(fd);

    return 1;
}