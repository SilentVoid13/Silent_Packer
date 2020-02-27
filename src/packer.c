//
// Created by silentvoid on 2/25/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "packer.h"
#include "elf_allocation.h"
#include "cipher_functions.h"
#include "section_insertion.h"
#include "write_elf.h"

#include "log.h"

/*
 * Mapping file in memory using mmap
 *
 */

int allocate_file(char *file, void **file_data, size_t *file_data_size) {
    int fd = open(file, O_RDONLY);
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    int size = lseek(fd, 0, SEEK_END);
    if(size < 0) {
        log_error("lseek() failure");
        return -1;
    }
    *file_data_size = size;

    *file_data = mmap(NULL, *file_data_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); // NOLINT(hicpp-signed-bitwise)
    if(*file_data == MAP_FAILED) {
        log_error("mmap() failure");
        return -1;
    }
    close(fd);

    return 1;
}

int pack_file(char *file, char *cipher, char *output) {
    void *file_data;
    size_t file_data_size;

    log_verbose("Allocating file in memory ...");
    if(allocate_file(file, &file_data, &file_data_size) == -1) {
        log_error("Error during file allocation");
        return -1;
    }

    log_verbose("Allocating ELF in memory ...");
    t_elf *elf = NULL;
    if(allocate_elf(&elf, file_data, file_data_size) == -1) {
        log_error("Error during ELF allocation");
        return -1;
    }

    // De-allocate mapped file as we don't need it anymore
    munmap(file_data, file_data_size);

    log_verbose("Encrypting .text section ...");
    if(encrypt_data(elf, cipher) == -1) {
        log_error("Error during ELF encryption");
        return -1;
    }

    log_verbose("Inserting new section in ELF ...");
    if(insert_section(elf) == -1) {
        log_error("Error during section insertion");
        return -1;
    }

    char *filename;
    if(output != NULL)
        filename = output;
    else
        filename = "packed.elf";

    log_verbose("Writing Packed ELF to file ...");
    if(write_elf(elf, filename) == -1) {
        log_error("Error during new ELF writing");
        return -1;
    }

    log_success("File %s packed into %s !", file, filename);

    return 1;
}
