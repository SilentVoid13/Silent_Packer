//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_allocation.h"
#include "pe_deallocation.h"
#include "file_functions.h"

#include "log.h"

int allocate_pe_dos_header(t_pe *pe, void *file_data, size_t file_data_size) {
    if(file_data_size < sizeof(IMAGE_DOS_HEADER)) {
        log_error("Total file size is less than DOS Header size");
        return -1;
    }

    // PE32 and PE32+ have the same DOS Header

    ((t_pe32 *)pe)->dos_header = malloc(sizeof(IMAGE_DOS_HEADER));
    if (((t_pe32 *)pe)->dos_header == NULL) {
        deallocate_pe_dos_header(pe);
        log_error("malloc() failure");
        return -1;
    }
    memcpy(((t_pe32 *)pe)->dos_header, file_data, sizeof(IMAGE_DOS_HEADER));

    // 0x5a4d
    if (((t_pe32 *)pe)->dos_header->e_magic != DOSMAG) {
        deallocate_pe_dos_header(pe);
        log_error("Magic bytes do not match PE file");
        return -1;
    }

    // TODO : check architecture like in ELF

    return 1;
}

int allocate_pe_dos_stub(t_pe *pe, void *file_data) {
    size_t dos_stub_size = ((t_pe32 *)pe)->dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER);

    // PE32 and PE32+ have the same DOS Stub

    ((t_pe32 *)pe)->dos_stub = malloc(dos_stub_size);
    if(((t_pe32 *)pe)->dos_stub == NULL) {
        deallocate_pe_dos_stub(pe);
        log_error("malloc() failure");
        return -1;
    }
    memcpy(((t_pe32 *)pe)->dos_stub, file_data + sizeof(IMAGE_DOS_HEADER), dos_stub_size);

    return 1;
}

int allocate_pe_pe_header(t_pe *pe, void *file_data, size_t file_data_size) {
    if(pe->s_type == PE32) {
        if (file_data_size < sizeof(IMAGE_NT_HEADERS32)) {
            log_error("Total file size is less than PE Header size");
            return -1;
        }

        ((t_pe32 *)pe)->pe_header = malloc(sizeof(IMAGE_NT_HEADERS32));
        if (((t_pe32 *)pe)->pe_header == NULL) {
            deallocate_pe_pe_header(pe);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_pe32 *)pe)->pe_header, file_data + ((t_pe32 *)pe)->dos_header->e_lfanew, sizeof(IMAGE_NT_HEADERS32));

        if ((((t_pe32 *)pe)->pe_header->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) { // NOLINT(hicpp-signed-bitwise)
            deallocate_pe_pe_header(pe);
            log_error("The file is not an executable");
            return -1;
        }

        if (((t_pe32 *)pe)->pe_header->FileHeader.SizeOfOptionalHeader == 0) {
            deallocate_pe_pe_header(pe);
            log_error("The file is an OBJ file");
            return -1;
        }

        if (((t_pe32 *)pe)->pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            deallocate_pe_pe_header(pe);
            log_error("File is not an executable image");
            return -1;
        }
    }
    else {
        if (file_data_size < sizeof(IMAGE_NT_HEADERS64)) {
            log_error("Total file size is less than PE Header size");
            return -1;
        }

        ((t_pe64 *)pe)->pe_header = malloc(sizeof(IMAGE_NT_HEADERS64));
        if (((t_pe64 *)pe)->pe_header == NULL) {
            deallocate_pe_pe_header(pe);
            log_error("malloc() failure");
            return -1;
        }
        memcpy(((t_pe64 *)pe)->pe_header, file_data + ((t_pe64 *)pe)->dos_header->e_lfanew, sizeof(IMAGE_NT_HEADERS64));

        if ((((t_pe64 *)pe)->pe_header->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) { // NOLINT(hicpp-signed-bitwise)
            deallocate_pe_pe_header(pe);
            log_error("The file is not an executable");
            return -1;
        }

        if (((t_pe64 *)pe)->pe_header->FileHeader.SizeOfOptionalHeader == 0) {
            deallocate_pe_pe_header(pe);
            log_error("The file is an OBJ file");
            return -1;
        }

        if (((t_pe64 *)pe)->pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
            deallocate_pe_pe_header(pe);
            log_error("File is not an executable image");
            return -1;
        }
    }

    return 1;
}

int allocate_pe_sections_headers(t_pe *pe, void *file_data, size_t file_data_size) {
    if(pe->s_type == PE32) {
        size_t pe_sections_header_size = sizeof(IMAGE_SECTION_HEADER) * ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections;

        ((t_pe32 *)pe)->section_header = malloc(pe_sections_header_size);
        if (((t_pe32 *)pe)->section_header == NULL) {
            deallocate_pe_sections_headers(pe);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_pe32 *)pe)->section_header, 0, pe_sections_header_size);

        // TODO: Check for IMAGE_SCN_CNT_UNINITIALIZED_DATA type like in elf_allocation
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (file_data_size <
                ((t_pe32 *)pe)->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS32) + (i * sizeof(IMAGE_SECTION_HEADER))) {
                deallocate_pe_sections_headers(pe);
                log_error("Total file size is inferior to PE section header size");
                return -1;
            }
            memcpy(&(((t_pe32 *)pe)->section_header[i]), file_data + ((t_pe32 *)pe)->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS32) +
                                             (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
        }
    }
    else {
        size_t pe_sections_header_size = sizeof(IMAGE_SECTION_HEADER) * ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections;

        ((t_pe64 *)pe)->section_header = malloc(pe_sections_header_size);
        if (((t_pe64 *)pe)->section_header == NULL) {
            deallocate_pe_sections_headers(pe);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_pe64 *)pe)->section_header, 0, pe_sections_header_size);

        // TODO: Check for IMAGE_SCN_CNT_UNINITIALIZED_DATA type like in elf_allocation
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (file_data_size <
                ((t_pe64 *)pe)->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (i * sizeof(IMAGE_SECTION_HEADER))) {
                deallocate_pe_sections_headers(pe);
                log_error("Total file size is inferior to PE section header size");
                return -1;
            }
            memcpy(&(((t_pe64 *)pe)->section_header[i]), file_data + ((t_pe64 *)pe)->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS64) +
                                                         (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
        }
    }

    return 1;
}

int allocate_pe_sections_data(t_pe *pe, void *file_data, size_t file_data_size) {
    if(pe->s_type == PE32) {
        size_t section_data_size = sizeof(char *) * ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections;

        ((t_pe32 *)pe)->section_data = malloc(section_data_size);
        if (((t_pe32 *)pe)->section_data == NULL) {
            deallocate_pe_sections_data(pe);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_pe32 *)pe)->section_data, 0, section_data_size);

        // TODO: Free all the malloc section_data_data up to i
        size_t pe_section_data_size;
        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (file_data_size < ((t_pe32 *)pe)->section_header[i].PointerToRawData) {
                log_error("Total file size is less than section data offset");
                return -1;
            }
            pe_section_data_size = ((t_pe32 *)pe)->section_header[i].SizeOfRawData;

            ((t_pe32 *)pe)->section_data[i] = malloc(pe_section_data_size);
            if (((t_pe32 *)pe)->section_data[i] == NULL) {
                free(((t_pe32 *)pe)->section_data[i]);
                log_error("malloc() error");
                return -1;
            }
            memset(((t_pe32 *)pe)->section_data[i], 0, pe_section_data_size);
            memcpy(((t_pe32 *)pe)->section_data[i], file_data + ((t_pe32 *)pe)->section_header[i].PointerToRawData, pe_section_data_size);
        }
    }
    else {
        size_t section_data_size = sizeof(char *) * ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections;

        ((t_pe64 *)pe)->section_data = malloc(section_data_size);
        if (((t_pe64 *)pe)->section_data == NULL) {
            deallocate_pe_sections_data(pe);
            log_error("malloc() failure");
            return -1;
        }
        memset(((t_pe64 *)pe)->section_data, 0, section_data_size);

        size_t pe_section_data_size;
        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            if (file_data_size < ((t_pe64 *)pe)->section_header[i].PointerToRawData) {
                log_error("Total file size is less than section data offset");
                return -1;
            }
            pe_section_data_size = ((t_pe64 *)pe)->section_header[i].SizeOfRawData;

            ((t_pe64 *)pe)->section_data[i] = malloc(pe_section_data_size);
            if (((t_pe64 *)pe)->section_data[i] == NULL) {
                free(((t_pe64 *)pe)->section_data[i]);
                log_error("malloc() error");
                return -1;
            }
            memset(((t_pe64 *)pe)->section_data[i], 0, pe_section_data_size);
            memcpy(((t_pe64 *)pe)->section_data[i], file_data + ((t_pe64 *)pe)->section_header[i].PointerToRawData, pe_section_data_size);
        }
    }

    return 1;
}

void print_pe_info(t_pe *pe) {
    if(pe->s_type == PE32) {
        printf("e_lfanew : %d\n", ((t_pe32 *)pe)->dos_header->e_lfanew);
        printf("section_header size : %ld\n",
               sizeof(IMAGE_SECTION_HEADER) * ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections);

        for (int i = 0; i < ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            printf("section_offset: %d\n", ((t_pe32 *)pe)->section_header[i].PointerToRawData);
            printf("section_size : %d\n", ((t_pe32 *)pe)->section_header[i].SizeOfRawData);
            if (i != ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections - 1) {
                printf("next_section_offset : %d\n", ((t_pe32 *)pe)->section_header[i + 1].PointerToRawData);
            }
            printf("\n");
        }
    }
    else {
        printf("e_lfanew : %d\n", ((t_pe64 *)pe)->dos_header->e_lfanew);
        printf("section_header size : %ld\n",
               sizeof(IMAGE_SECTION_HEADER) * ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections);

        for (int i = 0; i < ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections; i++) {
            printf("section_offset: %d\n", ((t_pe64 *)pe)->section_header[i].PointerToRawData);
            printf("section_size : %d\n", ((t_pe64 *)pe)->section_header[i].SizeOfRawData);
            if (i != ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections - 1) {
                printf("next_section_offset : %d\n", ((t_pe64 *)pe)->section_header[i + 1].PointerToRawData);
            }
            printf("\n");
        }
    }
}

int allocate_pe(t_pe **pe, void *file_data, size_t file_data_size, int arch) {
    size_t t_pe_size;
    if(arch == x32_ARCH) {
        t_pe_size = sizeof(t_pe32);
    }
    else {
        t_pe_size = sizeof(t_pe64);
    }

    *pe = malloc(t_pe_size);
    if(*pe == NULL) {
        deallocate_pe_struct(*pe);
        log_error("malloc() failure");
        return -1;
    }
    memset(*pe, 0, t_pe_size);

    t_pe type_pe;
    if(arch == 32) {
        type_pe.s_type = PE32;
        ((t_pe32 *)(*pe))->type_header = type_pe;
    }
    else {
        type_pe.s_type = PE64;
        ((t_pe32 *)(*pe))->type_header = type_pe;
    }

    log_verbose("Allocating DOS Header ...");
    if(allocate_pe_dos_header(*pe, file_data, file_data_size) == -1) {
        deallocate_pe_struct(*pe);
        log_error("Error during DOS Header allocation");
        return -1;
    }

    log_verbose("Allocating DOS Stub ...");
    if(allocate_pe_dos_stub(*pe, file_data) == -1) {
        deallocate_pe_dos_header(*pe);
        deallocate_pe_struct(*pe);
        log_error("Error during DOS Stub allocation");
        return -1;
    }

    log_verbose("Allocating PE Header ...");
    if(allocate_pe_pe_header(*pe, file_data, file_data_size) == -1) {
        deallocate_pe_dos_header(*pe);
        deallocate_pe_dos_stub(*pe);
        deallocate_pe_struct(*pe);
        log_error("Error during PE Header allocation");
        return -1;
    }

    log_verbose("Allocating Sections Headers ...");
    if(allocate_pe_sections_headers(*pe, file_data, file_data_size) == -1) {
        deallocate_pe_dos_header(*pe);
        deallocate_pe_dos_stub(*pe);
        deallocate_pe_pe_header(*pe);
        deallocate_pe_struct(*pe);
        log_error("Error during Section Headers allocation");
        return -1;
    }

    log_verbose("Allocating Sections Data ...");
    if(allocate_pe_sections_data(*pe, file_data, file_data_size) == -1) {
        deallocate_pe_dos_header(*pe);
        deallocate_pe_dos_stub(*pe);
        deallocate_pe_pe_header(*pe);
        deallocate_pe_sections_headers(*pe);
        deallocate_pe_struct(*pe);
        log_error("Error during Section Data allocation");
        return -1;
    }

    //print_pe_info(*pe);

    return 1;
}