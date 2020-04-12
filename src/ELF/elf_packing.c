//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_packing.h"
#include "elf_allocation.h"
#include "elf_deallocation.h"
#include "file_functions.h"
#include "elf_encryption.h"
#include "elf_writing.h"
#include "elf_packing_method.h"
#include "packer_config.h"

#include "log.h"

/*
 * Basic ELF structure
 *
 * -----------
 * ELF Header
 * -----------
 * Program header table
 * -----------
 * [section_data]
 * .text
 * .rodata
 * .shstrtab (string table section names)
 * [...]
 * -----------
 * Section header table containing multiple section headers
 * .text header
 * -----------
 *
*/

int pack_elf(char *file, char *file_data, size_t file_data_size, char *output) {
    log_verbose("Detected arch : x%d", packer_config.arch);
    log_info("Allocating ELF in memory ...");

    t_elf *elf = NULL;
    if(allocate_elf(&elf, file_data, file_data_size) == -1) {
        munmap(file_data, file_data_size);
        log_error("Error during ELF allocation");
        return -1;
    }

    // De-allocate mapped file as we don't need it anymore
    munmap(file_data, file_data_size);

    log_info("Encrypting .text section ...");
    if(encrypt_elf(elf) == -1) {
        deallocate_elf(elf);
        log_error("Error during ELF encryption");
        return -1;
    }

    log_info("Packing using specified method ...");
    if(elf_pack_using_method(elf) == -1) {
        deallocate_elf(elf);
        log_error("Error during ELF packing");
        return -1;
    }

    char *filename;
    if(output != NULL)
        filename = output;
    else
        filename = "packed.elf";

    log_info("Writing Packed ELF to file ...");
    if(write_elf(elf, filename) == -1) {
        deallocate_elf(elf);
        log_error("Error during new ELF writing");
        return -1;
    }

    log_success("File %s packed into %s !", file, filename);
    deallocate_elf(elf);

    return 1;
}
