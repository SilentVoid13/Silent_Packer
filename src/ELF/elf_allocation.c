//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_allocation.h"
#include "elf_deallocation.h"
#include "file_functions.h"
#include "packer_config.h"

#include "log.h"

int allocate_elf_elf_header(t_elf *elf, void *file_data, size_t file_data_size) {
    // http://www.openvirtualization.org/documentation/structElf64__Ehdr.html

    if(elf->s_type == ELF32) {
        if (file_data_size < sizeof(Elf32_Ehdr)) {
            log_error("Total file size is less than ELF Header size");
            return -1;
        }

        ((t_elf32 *)elf)->elf_header = malloc(sizeof(Elf32_Ehdr));
        if (((t_elf32 *)elf)->elf_header == NULL) {
            deallocate_elf_elf_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_elf32 *)elf)->elf_header, file_data, sizeof(Elf32_Ehdr));

        // https://code.woboq.org/userspace/glibc/elf/elf.h.html
        // "\177ELF"
        if (strncmp((char *) ((t_elf32 *)elf)->elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
            deallocate_elf_elf_header(elf);
            log_error("Magic bytes does not match ELF bytes");
            return -1;
        }

        if (((t_elf32 *)elf)->elf_header->e_machine != EM_386) {
            deallocate_elf_elf_header(elf);
            log_error("File is not a x32 executable");
            return -1;
        }
    }
    else {
        if (file_data_size < sizeof(Elf64_Ehdr)) {
            log_error("Total file size is less than ELF Header size");
            return -1;
        }

        ((t_elf64 *)elf)->elf_header = malloc(sizeof(Elf64_Ehdr));
        if (((t_elf64 *)elf)->elf_header == NULL) {
            deallocate_elf_elf_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_elf64 *)elf)->elf_header, file_data, sizeof(Elf64_Ehdr));

        // https://code.woboq.org/userspace/glibc/elf/elf.h.html
        // "\177ELF"
        if (strncmp((char *) ((t_elf64 *)elf)->elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
            deallocate_elf_elf_header(elf);
            log_error("Magic bytes does not match ELF bytes");
            return -1;
        }

        if (((t_elf64 *)elf)->elf_header->e_machine != EM_X86_64) {
            deallocate_elf_elf_header(elf);
            log_error("File is not a x64 executable");
            return -1;
        }
    }

    return 1;
}

int allocate_elf_program_header(t_elf *elf, void *file_data, size_t file_data_size) {
    if(elf->s_type == ELF32) {
        size_t elf_program_header_size = sizeof(Elf32_Phdr) * ((t_elf32 *)elf)->elf_header->e_phnum;
        if (file_data_size < sizeof(Elf32_Ehdr) + elf_program_header_size) {
            log_error("Total file size is inferior to ELF Header + Program Header size");
            return -1;
        }

        ((t_elf32 *)elf)->prog_header = malloc(elf_program_header_size);
        if (((t_elf32 *)elf)->prog_header == NULL) {
            deallocate_elf_program_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_elf32 *)elf)->prog_header, file_data + ((t_elf32 *)elf)->elf_header->e_phoff, elf_program_header_size);
    }
    else {
        size_t elf_program_header_size = sizeof(Elf64_Phdr) * ((t_elf64 *)elf)->elf_header->e_phnum;
        if (file_data_size < sizeof(Elf64_Ehdr) + elf_program_header_size) {
            log_error("Total file size is inferior to ELF Header + Program Header size");
            return -1;
        }

        ((t_elf64 *)elf)->prog_header = malloc(elf_program_header_size);
        if (((t_elf64 *)elf)->prog_header == NULL) {
            deallocate_elf_program_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_elf64 *)elf)->prog_header, file_data + ((t_elf64 *)elf)->elf_header->e_phoff, elf_program_header_size);
    }

    return 1;
}

int allocate_elf_sections_header(t_elf *elf, void *file_data, size_t file_data_size) {
    if(elf->s_type == ELF32) {
        size_t elf_sections_header_size = sizeof(Elf32_Shdr) * ((t_elf32 *)elf)->elf_header->e_shnum;

        ((t_elf32 *)elf)->section_header = malloc(elf_sections_header_size);
        if (((t_elf32 *)elf)->section_header == NULL) {
            deallocate_elf_sections_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_elf32 *)elf)->section_header, 0, elf_sections_header_size);

        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            if (file_data_size < ((t_elf32 *)elf)->elf_header->e_shoff + (i * sizeof(Elf32_Shdr))) {
                deallocate_elf_sections_header(elf);
                log_error("Total file size is inferior to ELF section header size");
                return -1;
            }
            memcpy(&(((t_elf32 *)elf)->section_header[i]), file_data + ((t_elf32 *)elf)->elf_header->e_shoff + (i * sizeof(Elf32_Shdr)),
                   sizeof(Elf32_Shdr));
        }
    }
    else {
        size_t elf_sections_header_size = sizeof(Elf64_Shdr) * ((t_elf64 *)elf)->elf_header->e_shnum;

        ((t_elf64 *)elf)->section_header = malloc(elf_sections_header_size);
        if (((t_elf64 *)elf)->section_header == NULL) {
            deallocate_elf_sections_header(elf);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_elf64 *)elf)->section_header, 0, elf_sections_header_size);

        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            if (file_data_size < ((t_elf64 *)elf)->elf_header->e_shoff + (i * sizeof(Elf64_Shdr))) {
                deallocate_elf_sections_header(elf);
                log_error("Total file size is inferior to ELF section header size");
                return -1;
            }
            memcpy(&(((t_elf64 *)elf)->section_header[i]), file_data + ((t_elf64 *)elf)->elf_header->e_shoff + (i * sizeof(Elf64_Shdr)),
                   sizeof(Elf64_Shdr));
        }
    }

    return 1;
}

int allocate_elf_sections_data(t_elf *elf, void *file_data, size_t file_data_size) {
    if(elf->s_type == ELF32) {
        ((t_elf32 *)elf)->section_data = malloc(sizeof(char *) * ((t_elf32 *)elf)->elf_header->e_shnum);
        if (((t_elf32 *)elf)->section_data == NULL) {
            deallocate_elf_sections_data(elf);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_elf32 *)elf)->section_data, 0, sizeof(char *) * ((t_elf32 *)elf)->elf_header->e_shnum);

        // TODO: Free all the malloc section_data_data up to i
        size_t elf_section_data_size;
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            // Section that occupies no space in file
            if (((t_elf32 *)elf)->section_header[i].sh_type == SHT_NOBITS) {
                ((t_elf32 *)elf)->section_data[i] = NULL;
            } else {
                if (file_data_size < ((t_elf32 *)elf)->section_header[i].sh_offset) {
                    log_error("Total file size is less than section data offset");
                    return -1;
                }

                elf_section_data_size = ((t_elf32 *)elf)->section_header[i].sh_size;

                ((t_elf32 *)elf)->section_data[i] = malloc(elf_section_data_size);
                if (((t_elf32 *)elf)->section_data[i] == NULL) {
                    free(((t_elf32 *)elf)->section_data[i]);
                    log_error("malloc() failure");
                    return -1;
                }
                memset(((t_elf32 *)elf)->section_data[i], 0, elf_section_data_size);
                memcpy(((t_elf32 *)elf)->section_data[i], file_data + ((t_elf32 *)elf)->section_header[i].sh_offset, elf_section_data_size);
            }
        }
    }
    else {
        ((t_elf64 *)elf)->section_data = malloc(sizeof(char *) * ((t_elf64 *)elf)->elf_header->e_shnum);
        if (((t_elf64 *)elf)->section_data == NULL) {
            deallocate_elf_sections_data(elf);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_elf64 *)elf)->section_data, 0, sizeof(char *) * ((t_elf64 *)elf)->elf_header->e_shnum);

        size_t elf_section_data_size;
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            // Section that occupies no space in file
            if (((t_elf64 *)elf)->section_header[i].sh_type == SHT_NOBITS) {
                ((t_elf64 *)elf)->section_data[i] = NULL;
            } else {
                if (file_data_size < ((t_elf64 *)elf)->section_header[i].sh_offset) {
                    log_error("Total file size is less than section data offset");
                    return -1;
                }

                elf_section_data_size = ((t_elf64 *)elf)->section_header[i].sh_size;

                ((t_elf64 *)elf)->section_data[i] = malloc(elf_section_data_size);
                if (((t_elf64 *)elf)->section_data[i] == NULL) {
                    free(((t_elf64 *)elf)->section_data[i]);
                    log_error("malloc() failure");
                    return -1;
                }
                memset(((t_elf64 *)elf)->section_data[i], 0, elf_section_data_size);
                memcpy(((t_elf64 *)elf)->section_data[i], file_data + ((t_elf64 *)elf)->section_header[i].sh_offset, elf_section_data_size);
            }
        }
    }

    return 1;
}

int allocate_elf(t_elf **elf, void *file_data, size_t file_data_size) {
    size_t t_elf_size;

    if(packer_config.arch == x32_ARCH)
        t_elf_size = sizeof(t_elf32);
    else
        t_elf_size = sizeof(t_elf64);

    *elf = malloc(t_elf_size);
    if(*elf == NULL) {
        deallocate_elf_struct(*elf);
        log_error("malloc() failure");
        return -1;
    }
    memset(*elf, 0, t_elf_size);

    t_elf type_pe;
    if(packer_config.arch == 32) {
        type_pe.s_type = ELF32;
        ((t_elf32 *)(*elf))->type_header = type_pe;
    }
    else {
        type_pe.s_type = ELF64;
        ((t_elf64 *)(*elf))->type_header = type_pe;
    }

    log_verbose("Allocating ELF Header ...");

    if(allocate_elf_elf_header(*elf, file_data, file_data_size) == -1) {
        log_error("Error during ELF Header allocation");
        return -1;
    }

    log_verbose("Allocating Program Header ...");

    if(allocate_elf_program_header(*elf, file_data, file_data_size) == -1) {
        deallocate_elf_elf_header(*elf);
        log_error("Error during Program Header allocation");
        return -1;
    }

    log_verbose("Allocating Sections Headers ...");

    if(allocate_elf_sections_header(*elf, file_data, file_data_size) == -1) {
        deallocate_elf_elf_header(*elf);
        deallocate_elf_program_header(*elf);
        log_error("Error during Section Header allocation");
        return -1;
    }

    log_verbose("Allocating Sections Data ...");

    if(allocate_elf_sections_data(*elf, file_data, file_data_size) == -1) {
        deallocate_elf_elf_header(*elf);
        deallocate_elf_program_header(*elf);
        deallocate_elf_sections_header(*elf);
        log_error("Error during Section Data allocation");
        return -1;
    }

    return 1;
}
