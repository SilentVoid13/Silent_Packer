//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "write_pe.h"
#include "pe_allocation.h"
#include "file_functions.h"

#include "log.h"

int write_pe(t_pe64 *pe, char *filename) {
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0755); // NOLINT(hicpp-signed-bitwise)
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    // We write the program header
    log_verbose("Writing DOS header ...");
    write_to_file(fd, pe->dos_header, sizeof(IMAGE_DOS_HEADER));

    log_verbose("Writing DOS Stub ...");
    write_to_file(fd, pe->dos_stub, pe->dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER));

    log_verbose("Writing PE header ...");
    write_to_file(fd, pe->pe_header, sizeof(IMAGE_NT_HEADERS64));

    log_verbose("Writing Sections headers ...");
    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        write_to_file(fd, &(pe->section_header[i]), sizeof(IMAGE_SECTION_HEADER));
    }
    add_zero_padding(fd, pe->section_header[0].PointerToRawData);

    log_verbose("Writing Sections data ...");
    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        write_to_file(fd, pe->section_data[i], pe->section_header[i].SizeOfRawData);
    }

    close(fd);

    return 1;
}
