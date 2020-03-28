//
// Created by silentvoid on 3/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_struct.h"
#include "pe_section_insertion.h"
#include "pe_allocation.h"
#include "pe_functions.h"
#include "loader_functions.h"

#include "log.h"

void print_new_section_header_info(IMAGE_SECTION_HEADER *section_header) {
    printf("VirtualAddress : %d\n", section_header->VirtualAddress);
    printf("VirtualSize : %d\n", section_header->Misc.VirtualSize);
    printf("SizeOfRawData : %d\n", section_header->SizeOfRawData);
}

int add_new_pe_section_header(t_pe64 *pe) {
    int sections_count = pe->pe_header->FileHeader.NumberOfSections;
    size_t new_section_header_size = sections_count * sizeof(IMAGE_SECTION_HEADER) + sizeof(IMAGE_SECTION_HEADER);
    IMAGE_SECTION_HEADER *new_section_header = realloc(pe->section_header, new_section_header_size);
    if(new_section_header == NULL) {
        log_error("realloc() failure\n");
        return -1;
    }
    pe->section_header = new_section_header;

    IMAGE_SECTION_HEADER new_header;
    memset(&new_header, 0, sizeof(IMAGE_SECTION_HEADER));

    char *section_name = ".dec";
    memset(new_header.Name, 0, sizeof(new_header.Name));
    memcpy(new_header.Name, section_name, strlen(section_name));

    new_header.VirtualAddress = pe->pe_header->OptionalHeader.SizeOfImage;
    new_header.PointerToRawData = pe->section_header[sections_count-1].PointerToRawData + pe->section_header[sections_count-1].SizeOfRawData;

    new_header.Misc.VirtualSize = loader_size;
    new_header.SizeOfRawData = pe->pe_header->OptionalHeader.FileAlignment;

    new_header.Characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE; // NOLINT(hicpp-signed-bitwise)

    //print_new_section_header_info(&new_header);

    memcpy(&(pe->section_header[sections_count]), &new_header, sizeof(IMAGE_SECTION_HEADER));

    return 1;
}

int set_new_pe_header_values(t_pe64 *pe) {
    int old_sections_count = pe->pe_header->FileHeader.NumberOfSections;
    pe->pe_header->FileHeader.NumberOfSections += 1;

    pe->pe_header->OptionalHeader.SizeOfCode += pe->section_header[old_sections_count-1].SizeOfRawData;
    pe->pe_header->OptionalHeader.SizeOfHeaders += sizeof(IMAGE_SECTION_HEADER);
    pe->pe_header->OptionalHeader.SizeOfImage += loader_size;

    return 1;
}

int add_new_pe_section_data(t_pe64 *pe) {
    int sections_count = pe->pe_header->FileHeader.NumberOfSections;
    char **new_section_data = realloc(pe->section_data, sizeof(char *) * sections_count);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }
    pe->section_data = new_section_data;
    //memset(pe->section_data[sections_count-1], 0, pe->section_header[sections_count-1].SizeOfRawData);

    // For ASM
    loader_offset = pe->section_header[sections_count - 1].VirtualAddress;

    char *loader = patch_loader();
    if(loader == NULL) {
        log_error("Error during loader patching");
        return -1;
    }
    char *new_section = malloc(pe->section_header[sections_count-1].SizeOfRawData);
    memset(new_section, 0, pe->section_header[sections_count-1].SizeOfRawData);
    memcpy(new_section, loader, loader_size);
    pe->section_data[sections_count-1] = new_section;
    free(loader);

    return 1;
}

int pe_insert_section(t_pe64 *pe) {
    int text_section_index = find_pe_text_section(pe);
    if(text_section_index == -1) {
        log_error("Couldn't find .text section");
        return -1;
    }
    add_pe_section_permission(pe, text_section_index, IMAGE_SCN_MEM_WRITE);

    if(add_new_pe_section_header(pe) == -1) {
        log_error("Error during new Section Header insertion");
        return -1;
    }

    if(set_new_pe_header_values(pe) == -1) {
        log_error("Couldn't set new PE Header values");
        return -1;
    }

    if(add_new_pe_section_data(pe) == -1) {
        log_error("Error during new Section Data insertion");
        return -1;
    }

    set_new_pe_entry_to_section(pe, pe->pe_header->FileHeader.NumberOfSections-1);

    return 1;
}