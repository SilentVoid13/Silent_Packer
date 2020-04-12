//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "file_functions.h"
#include "elf_allocation.h"
#include "pe_struct.h"

#include "log.h"

// To keep track of the bytes written for the padding
size_t offset = 0;

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
        close(fd);
        log_error("lseek() failure");
        return -1;
    }
    *file_data_size = size;

    *file_data = mmap(NULL, *file_data_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); // NOLINT(hicpp-signed-bitwise)
    if(*file_data == MAP_FAILED) {
        close(fd);
        log_error("mmap() failure");
        return -1;
    }
    close(fd);

    return 1;
}

int write_to_file(int fd, void *data, size_t data_size) {
    size_t n_bytes;
    if((n_bytes = write(fd, data, data_size)) != data_size) {
        log_error("write() failure");
        return -1;
    }
    offset += n_bytes;

    return 1;
}

int dump_to_file(char *filename, char *data, size_t data_size) {
    FILE *dump;
    dump = fopen(filename, "w");
    if(dump == NULL) {
        log_error("Error dumping file");
        return -1;
    }
    fwrite(data, data_size, 1, dump);
    fclose(dump);

    return 1;
}

void add_zero_padding(int fd, size_t end_offset) {
    char c = 0;
    while(offset < end_offset) {
        write_to_file(fd, &c, sizeof(c));
    }
}

int check_magic_bytes(char *file_data, size_t file_data_size) {
    // TODO: Random value - See for a coherent value
    if(file_data_size < 50) {
        log_error("Invalid file size");
        return UNKNOWN_FILE;
    }

    if(strncmp(file_data, ELFMAG, SELFMAG) == 0) {
        return ELF_FILE;
    }
    else if(strncmp(file_data, STR_DOSMAG, SSTR_DOSMAG) == 0) {
        return PE_FILE;
    }
    else {
        return UNKNOWN_FILE;
    }
}

int get_elf_arch(const char *file_data, size_t file_data_size) {
    // TODO: Maybe improve this
    if(file_data_size < sizeof(Elf32_Ehdr)) {
        log_error("Invalid file size");
        return UNKNOWN_ARCH;
    }

    if(file_data[EI_CLASS] == ELFCLASS32) {
        return x32_ARCH;
    }
    else if(file_data[EI_CLASS] == ELFCLASS64) {
        return x64_ARCH;
    }
    else {
        return UNKNOWN_ARCH;
    }
}

int get_pe_arch(const char *file_data, size_t file_data_size) {
    // TODO: Maybe improve this
    if(file_data_size < sizeof(IMAGE_DOS_HEADER) + DOS_STUB_SIZE + sizeof(IMAGE_NT_HEADERS32)) {
        log_error("Invalid file size");
        return UNKNOWN_ARCH;
    }

    size_t machine_type_offset = sizeof(IMAGE_DOS_HEADER) + DOS_STUB_SIZE + sizeof(uint32_t) + sizeof(IMAGE_FILE_HEADER);
    uint16_t arch_value = *((uint16_t *)(file_data+machine_type_offset));

    if(arch_value == PE32MAG) {
        return x32_ARCH;
    }
    else if(arch_value == PE64MAG) {
        return x64_ARCH;
    }
    else {
        return UNKNOWN_ARCH;
    }
}