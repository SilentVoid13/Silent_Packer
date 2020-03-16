//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_allocation.h"

#include "log.h"

int allocate_pe_dos_header(t_pe *pe, void *file_data, size_t file_data_size) {
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

    // PE\0\0 or 0x00004550
    if(pe->dos_header->e_magic != PEMAG) {
        log_error("Magic bytes does not match PE bytes");
        return -1;
    }

    // TODO : check architecture like in ELF

    return 1;
}

int allocate_pe_pe_header(t_pe *pe, void *file_data, size_t file_data_size) {
    if(file_data_size < sizeof(IMAGE_NT_HEADERS32))  {
        log_error("Total file size is less than PE Header size");
        return -1;
    }

    pe->pe_header = malloc(sizeof(IMAGE_NT_HEADERS32));
    if(pe->pe_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memcpy(pe->pe_header, file_data + pe->dos_header->e_lfanew, sizeof(IMAGE_NT_HEADERS32));

    if((pe->pe_header->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) { // NOLINT(hicpp-signed-bitwise)
        log_error("The file is not an executable");
        return -1;
    }

    if(pe->pe_header->FileHeader.SizeOfOptionalHeader == 0) {
        log_error("The file is an OBJ file");
        return -1;
    }

    if(pe->pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        log_error("File is not an executable image");
        return -1;
    }

    return 1;
}

int allocate_pe_sections_headers(t_pe *pe, void *file_data, size_t file_data_size) {
    size_t pe_sections_header_size = sizeof(IMAGE_SECTION_HEADER) * pe->pe_header->FileHeader.NumberOfSections;

    pe->section_header = malloc(pe_sections_header_size);
    if(pe->section_header == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(pe->section_header, 0, pe_sections_header_size);

    for(int i = 0; i < pe->pe_header->FileHeader.NumberOfSections; i++) {
        if(file_data_size < pe->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS32) + (i * sizeof(IMAGE_SECTION_HEADER))) {
            log_error("Total file size is inferior to PE section header size");
            return -1;
        }
        memcpy(&(pe->section_header[i]), file_data + pe->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS32) + (i * sizeof(IMAGE_SECTION_HEADER)), pe_sections_header_size);
    }

    return 1;
}

int allocate_pe_sections_data(t_pe *pe, void *file_data, size_t file_data_size) {
    pe->section_data = malloc(sizeof(char *) * pe->pe_header->FileHeader.NumberOfSections);
    if(pe->section_data == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(pe->section_data, 0, sizeof(char *) * pe->pe_header->FileHeader.NumberOfSections);

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

int allocate_pe(t_pe **pe, void *file_data, size_t file_data_size) {
    size_t t_pe_size = sizeof(t_pe);

    *pe = malloc(t_pe_size);
    if(*pe == NULL) {
        log_error("malloc() failure");
        return -1;
    }
    memset(*pe, 0, t_pe_size);

    if(allocate_pe_dos_header(*pe, file_data, file_data_size) == -1) {
        log_error("Error during DOS Header allocation");
        return -1;
    }

    if(allocate_pe_pe_header(*pe, file_data, file_data_size) == -1) {
        log_error("Error during PE Header allocation");
        return -1;
    }

    if(allocate_pe_sections_headers(*pe, file_data, file_data_size) == -1) {
        log_error("Error during Section Data allocation");
        return -1;
    }

    return 1;
}