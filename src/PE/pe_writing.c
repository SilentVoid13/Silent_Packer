//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_writing.h"
#include "pe_allocation.h"
#include "file_functions.h"

#include "log.h"

int write_pe(t_pe *pe, char *filename) {
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0755); // NOLINT(hicpp-signed-bitwise)
    if(fd < 0) {
        log_error("open() failure");
        return -1;
    }

    if(pe->s_type == PE32) {
        log_verbose("Writing DOS header ...");
        write_to_file(fd, ((t_pe32 *)pe)->dos_header, sizeof(IMAGE_DOS_HEADER));

        log_verbose("Writing DOS Stub ...");
        write_to_file(fd, ((t_pe32 *)pe)->dos_stub, ((t_pe32 *)pe)->dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER));

        log_verbose("Writing PE header ...");
        write_to_file(fd, ((t_pe32 *)pe)->pe_header, sizeof(IMAGE_NT_HEADERS32));

        log_verbose("Writing Sections headers ...");
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            write_to_file(fd, &(((t_pe32 *)pe)->section_header[i]), sizeof(IMAGE_SECTION_HEADER));
        }
        add_zero_padding(fd, ((t_pe32 *)pe)->section_header[0].PointerToRawData);

        log_verbose("Writing Sections data ...");
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            write_to_file(fd, ((t_pe32 *)pe)->section_data[i], ((t_pe32 *)pe)->section_header[i].SizeOfRawData);
        }
    }
    else {
        log_verbose("Writing DOS header ...");
        write_to_file(fd, ((t_pe64 *)pe)->dos_header, sizeof(IMAGE_DOS_HEADER));

        log_verbose("Writing DOS Stub ...");
        write_to_file(fd, ((t_pe64 *)pe)->dos_stub, ((t_pe64 *)pe)->dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER));

        log_verbose("Writing PE header ...");
        write_to_file(fd, ((t_pe64 *)pe)->pe_header, sizeof(IMAGE_NT_HEADERS64));

        log_verbose("Writing Sections headers ...");
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            write_to_file(fd, &(((t_pe64 *)pe)->section_header[i]), sizeof(IMAGE_SECTION_HEADER));
        }
        add_zero_padding(fd, ((t_pe64 *)pe)->section_header[0].PointerToRawData);

        log_verbose("Writing Sections data ...");
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            write_to_file(fd, ((t_pe64 *)pe)->section_data[i], ((t_pe64 *)pe)->section_header[i].SizeOfRawData);
        }
    }

    close(fd);

    return 1;
}
