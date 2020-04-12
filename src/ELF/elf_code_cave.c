//
// Created by silentvoid on 3/7/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_code_cave.h"
#include "elf_functions.h"
#include "elf_allocation.h"
#include "elf_packing_method.h"
#include "loader_functions.h"
#include "file_functions.h"
#include "packer_config.h"
#include "all_elf_loaders_infos.h"

#include "log.h"

int find_elf_code_cave_index(t_elf *elf) {
    if(elf->s_type == ELF32) {
        for (int i = 0; i < ((t_elf32 *)elf)->elf_header->e_shnum; i++) {
            if (((t_elf32 *)elf)->section_header[i].sh_type == SHT_NOBITS) {
                continue;
            }
            unsigned int code_cave_size;

            // TODO: Maybe change that since it's relying on the fact that the section header is located after the section data (may not always be the case)
            if (i == ((t_elf32 *)elf)->elf_header->e_shnum - 1) {
                code_cave_size =
                        ((t_elf32 *)elf)->elf_header->e_shoff - (((t_elf32 *)elf)->section_header[i].sh_offset + ((t_elf32 *)elf)->section_header[i].sh_size);
            } else {
                code_cave_size = ((t_elf32 *)elf)->section_header[i + 1].sh_offset -
                                 (((t_elf32 *)elf)->section_header[i].sh_offset + ((t_elf32 *)elf)->section_header[i].sh_size);
            }

            if (code_cave_size > packer_config.loader_size) {
                int segment_index = find_elf_segment_index_of_section(elf, i);
                if (((t_elf32 *)elf)->prog_header[segment_index].p_type == PT_LOAD) {
                    log_verbose("Code Cave size : %d", code_cave_size);
                    return i;
                }
            }
        }
    }
    else {
        for (int i = 0; i < ((t_elf64 *)elf)->elf_header->e_shnum; i++) {
            if (((t_elf64 *)elf)->section_header[i].sh_type == SHT_NOBITS) {
                continue;
            }
            unsigned int code_cave_size;

            // TODO: Maybe change that since it's relying on the fact that the section header is located after the section data (may not always be the case)
            if (i == ((t_elf64 *)elf)->elf_header->e_shnum - 1) {
                code_cave_size =
                        ((t_elf64 *)elf)->elf_header->e_shoff - (((t_elf64 *)elf)->section_header[i].sh_offset + ((t_elf64 *)elf)->section_header[i].sh_size);
            } else {
                code_cave_size = ((t_elf64 *)elf)->section_header[i + 1].sh_offset -
                                 (((t_elf64 *)elf)->section_header[i].sh_offset + ((t_elf64 *)elf)->section_header[i].sh_size);
            }

            if (code_cave_size > packer_config.loader_size) {
                int segment_index = find_elf_segment_index_of_section(elf, i);
                if (((t_elf64 *)elf)->prog_header[segment_index].p_type == PT_LOAD) {
                    log_verbose("Code Cave size : %d", code_cave_size);
                    return i;
                }
            }
        }
    }

    return -1;
}

int set_new_elf_cave_segment_values(t_elf *elf, int segment_index) {
    if(elf->s_type == ELF32) {
        ((t_elf32 *)elf)->prog_header[segment_index].p_memsz += packer_config.loader_size;
        ((t_elf32 *)elf)->prog_header[segment_index].p_filesz += packer_config.loader_size;
    }
    else {
        ((t_elf64 *)elf)->prog_header[segment_index].p_memsz += packer_config.loader_size;
        ((t_elf64 *)elf)->prog_header[segment_index].p_filesz += packer_config.loader_size;
    }

    add_elf_segment_permission(elf, segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)
    add_elf_segment_permission(elf, segment_index, PF_X); // NOLINT(hicpp-signed-bitwise)

    int text_segment_index = find_elf_text_segment(elf);
    if(text_segment_index == -1) {
        log_error("Couldn't find .text segment");
        return -1;
    }
    add_elf_segment_permission(elf, text_segment_index, PF_W); // NOLINT(hicpp-signed-bitwise)

    return 1;
}

int elf_cave_insert_loader(t_elf *elf, int section_index, int old_section_size) {
    char *new_section_data;
    char *loader;

    if(elf->s_type == ELF32) {
        new_section_data = realloc(((t_elf32 *)elf)->section_data[section_index], old_section_size + packer_config.loader_size);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_elf32 *)elf)->section_data[section_index] = new_section_data;
        // For ASM
        loader_offset32 = ((t_elf32 *)elf)->section_header[section_index].sh_addr + old_section_size;

        loader = patch_loader();
        if(loader == NULL) {
            free(loader);
            log_error("Error during loader patching");
            return -1;
        }
        memcpy(new_section_data + old_section_size, loader, packer_config.loader_size);
    }
    else {
        new_section_data = realloc(((t_elf64 *)elf)->section_data[section_index], old_section_size + packer_config.loader_size);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_elf64 *)elf)->section_data[section_index] = new_section_data;
        // For ASM
        loader_offset64 = ((t_elf64 *)elf)->section_header[section_index].sh_addr + old_section_size;

        loader = patch_loader();
        if(loader == NULL) {
            free(loader);
            log_error("Error during loader patching");
            return -1;
        }
        memcpy(new_section_data + old_section_size, loader, packer_config.loader_size);
    }
    free(loader);

    return 1;
}

int elf_code_cave_injection(t_elf *elf) {

    log_verbose("Finding the code cave ...");

    int section_cave_index = find_elf_code_cave_index(elf);
    if(section_cave_index == -1) {
        log_error("Couldn't find any code cave in this ELF");
        return -1;
    }
    method_config.concerned_section = section_cave_index;

    int segment_index = find_elf_segment_index_of_section(elf, section_cave_index);
    if(segment_index == -1) {
        log_error("Couldn't find segment index");
        return -1;
    }

    log_verbose("Setting new segment values ...");

    if(set_new_elf_cave_segment_values(elf, segment_index) == -1) {
        log_error("Error during segment values modification");
        return -1;
    }

    log_verbose("Inserting the loader inside the code cave ...");

    int old_section_size;
    if(elf->s_type == ELF32)
        old_section_size = ((t_elf32 *)elf)->section_header[section_cave_index].sh_size;
    else
        old_section_size = ((t_elf64 *)elf)->section_header[section_cave_index].sh_size;

    if(elf_cave_insert_loader(elf, section_cave_index, old_section_size) == -1) {
        log_error("Error during Loader insertion");
        return -1;
    }

    log_verbose("Setting new ELF entry point ...");

    if(elf->s_type == ELF32) {
        Elf32_Addr loader_addr;
        loader_addr = ((t_elf32 *) elf)->section_header[section_cave_index].sh_addr + old_section_size;
        set_new_elf_entry_to_addr32(elf, loader_addr, section_cave_index, old_section_size);
    }
    else {
        Elf64_Addr loader_addr;
        loader_addr = ((t_elf64 *) elf)->section_header[section_cave_index].sh_addr + old_section_size;
        set_new_elf_entry_to_addr64(elf, loader_addr, section_cave_index, old_section_size);
    }

    return 1;
}
