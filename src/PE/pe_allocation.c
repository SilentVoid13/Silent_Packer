//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_allocation.h"

#include "log.h"

int allocate_pe_dos_header(t_pe64 *pe, void *file_data, size_t file_data_size) {
    if(file_data_size < sizeof(IMAGE_DOS_HEADER)) {
        log_error("Total file size is less than DOS Header size");
        return -1;
    }

    pe->dos_header = malloc(sizeof(IMAGE_DOS_HEADER));
    if(pe->dos_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(pe->dos_header, file_data, sizeof(IMAGE_DOS_HEADER));

    // 0x5a4d
    if(pe->dos_header->e_magic != DOSMAG) {
        log_error("Magic bytes does not match PE bytes");
        return -1;
    }

    // TODO : check architecture like in ELF

    return 1;
}

int allocate_pe_dos_stub(t_pe64 *pe, void *file_data) {
    size_t dos_stub_size = pe->dos_header->e_lfanew - sizeof(IMAGE_DOS_HEADER);

    pe->dos_stub = malloc(dos_stub_size);
    if(pe->dos_stub == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(pe->dos_stub, file_data + sizeof(IMAGE_DOS_HEADER), dos_stub_size);

    return 1;
}

int allocate_pe_pe_header(t_pe64 *pe, void *file_data, size_t file_data_size) {
    if(file_data_size < sizeof(IMAGE_NT_HEADERS64))  {
        log_error("Total file size is less than PE Header size");
        return -1;
    }

    pe->pe_header = malloc(sizeof(IMAGE_NT_HEADERS64));
    if(pe->pe_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(pe->pe_header, file_data + pe->dos_header->e_lfanew, sizeof(IMAGE_NT_HEADERS64));

    if((pe->pe_header->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) { // NOLINT(hicpp-signed-bitwise)
        log_error("The file is not an executable");
        return -1;
    }

    if(pe->pe_header->FileHeader.SizeOfOptionalHeader == 0) {
        log_error("The file is an OBJ file");
        return -1;
    }

    if(pe->pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        log_error("File is not an executable image");
        return -1;
    }

    return 1;
}

int allocate_pe_sections_headers(t_pe64 *pe, void *file_data, size_t file_data_size) {
    size_t pe_sections_header_size = sizeof(IMAGE_SECTION_HEADER) * pe->pe_header->FileHeader.NumberOfSections;

    pe->section_header = malloc(pe_sections_header_size);
    if(pe->section_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(pe->section_header, 0, pe_sections_header_size);

    // TODO: Check for IMAGE_SCN_CNT_UNINITIALIZED_DATA type like in elf_allocation
    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        if(file_data_size < pe->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (i * sizeof(IMAGE_SECTION_HEADER))) {
            log_error("Total file size is inferior to PE section header size");
            return -1;
        }
        memcpy(&(pe->section_header[i]), file_data + pe->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
    }

    return 1;
}

int allocate_pe_sections_data(t_pe64 *pe, void *file_data, size_t file_data_size) {
    size_t section_data_size = sizeof(char *) * pe->pe_header->FileHeader.NumberOfSections;

    pe->section_data = malloc(section_data_size);
    if(pe->section_data == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(pe->section_data, 0, section_data_size);

    size_t pe_section_data_size;
    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        if(file_data_size < pe->section_header[i].PointerToRawData) {
            log_error("Total file size is less than section data offset");
            return -1;
        }
        pe_section_data_size = pe->section_header[i].SizeOfRawData;

        pe->section_data[i] = malloc(pe_section_data_size);
        if(pe->section_data[i] == NULL) {
            log_error("malloc() error");
            return -1;
        }
        memset(pe->section_data[i], 0, pe_section_data_size);
        memcpy(pe->section_data[i], file_data + pe->section_header[i].PointerToRawData, pe_section_data_size);
    }

    return 1;
}

void print_pe_info(t_pe64 *pe) {
    printf("e_lfanew : %d\n", pe->dos_header->e_lfanew);

    printf("section_header size : %ld\n", sizeof(IMAGE_SECTION_HEADER) * pe->pe_header->FileHeader.NumberOfSections);

    printf("sizeof IMAGE_NT_HEADERS64 : %ld\n", sizeof(IMAGE_NT_HEADERS64));
    printf("sizeof IMAGE_NT_HEADERS32 : %ld\n", sizeof(IMAGE_NT_HEADERS32));
    printf("sizeof uint32_t : %ld\n", sizeof(uint32_t));
    printf("sizeof IMAGE_FILE_HEADER : %ld\n", sizeof(IMAGE_FILE_HEADER));
    printf("sizeof IMAGE_OPTIONAL_HEADER64 : %ld\n", sizeof(IMAGE_OPTIONAL_HEADER64));
    printf("sizeof IMAGE_OPTIONAL_HEADER32 : %ld\n", sizeof(IMAGE_OPTIONAL_HEADER32));

    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        printf("section_offset: %d\n", pe->section_header[i].PointerToRawData);
        printf("section_size : %d\n", pe->section_header[i].SizeOfRawData);
        if(i != pe->pe_header->FileHeader.NumberOfSections-1) {
            printf("next_section_offset : %d\n", pe->section_header[i+1].PointerToRawData);
        }
        printf("\n");
    }
}

int allocate_pe(t_pe64 **pe, void *file_data, size_t file_data_size) {
    size_t t_pe_size = sizeof(t_pe64);

    *pe = malloc(t_pe_size);
    if(*pe == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(*pe, 0, t_pe_size);

    log_verbose("Allocating DOS Header ...");
    if(allocate_pe_dos_header(*pe, file_data, file_data_size) == -1) {
        log_error("Error during DOS Header allocation");
        return -1;
    }

    log_verbose("Allocating DOS Stub ...");
    if(allocate_pe_dos_stub(*pe, file_data) == -1) {
        log_error("Error during DOS Stub allocation");
        return -1;
    }

    log_verbose("Allocating PE Header ...");
    if(allocate_pe_pe_header(*pe, file_data, file_data_size) == -1) {
        log_error("Error during PE Header allocation");
        return -1;
    }

    log_verbose("Allocating Sections Headers ...");
    if(allocate_pe_sections_headers(*pe, file_data, file_data_size) == -1) {
        log_error("Error during Section Headers allocation");
        return -1;
    }

    log_verbose("Allocating Sections Data ...");
    if(allocate_pe_sections_data(*pe, file_data, file_data_size) == -1) {
        log_error("Error during Section Data allocation");
        return -1;
    }

    //print_pe_info(*pe);

    return 1;
}