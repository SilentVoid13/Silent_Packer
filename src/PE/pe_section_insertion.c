#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by silentvoid on 3/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_section_insertion.h"
#include "pe_struct.h"
#include "pe_allocation.h"
#include "pe_functions.h"
#include "loader_functions.h"
#include "file_functions.h"
#include "packer_config.h"
#include "all_pe_loaders_infos.h"

#include "log.h"

void print_new_section_header_info(IMAGE_SECTION_HEADER *section_header) {
    printf("VirtualAddress : %d\n", section_header->VirtualAddress);
    printf("VirtualSize : %d\n", section_header->Misc.VirtualSize);
    printf("SizeOfRawData : %d\n", section_header->SizeOfRawData);
}

int add_new_pe_section_header(t_pe *pe) {
    int sections_count;
    if(pe->s_type == PE32) {
        sections_count = ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections;
        size_t new_section_header_size = sections_count * sizeof(IMAGE_SECTION_HEADER) + sizeof(IMAGE_SECTION_HEADER);
        IMAGE_SECTION_HEADER *new_section_header = realloc(((t_pe32 *)pe)->section_header, new_section_header_size);
        if (new_section_header == NULL) {
            log_error("realloc() failure\n");
            return -1;
        }
        ((t_pe32 *)pe)->section_header = new_section_header;
    }
    else {
        sections_count = ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections;
        size_t new_section_header_size = sections_count * sizeof(IMAGE_SECTION_HEADER) + sizeof(IMAGE_SECTION_HEADER);
        IMAGE_SECTION_HEADER *new_section_header = realloc(((t_pe64 *)pe)->section_header, new_section_header_size);
        if (new_section_header == NULL) {
            log_error("realloc() failure\n");
            return -1;
        }
        ((t_pe64 *)pe)->section_header = new_section_header;
    }

    IMAGE_SECTION_HEADER new_header;
    memset(&new_header, 0, sizeof(IMAGE_SECTION_HEADER));

    char *section_name = ".dec";
    memset(new_header.Name, 0, sizeof(new_header.Name));
    memcpy(new_header.Name, section_name, strlen(section_name));

    if(pe->s_type == PE32) {
        new_header.VirtualAddress = ((t_pe32 *)pe)->pe_header->OptionalHeader.SizeOfImage;
        new_header.PointerToRawData =   ((t_pe32 *)pe)->section_header[sections_count - 1].PointerToRawData +
                                        ((t_pe32 *)pe)->section_header[sections_count - 1].SizeOfRawData;

        new_header.Misc.VirtualSize = packer_config.loader_size;
        // TO SEE
        size_t raw_data_size = 0;
        while(raw_data_size < packer_config.loader_size) {
            raw_data_size += ((t_pe32 *)pe)->pe_header->OptionalHeader.FileAlignment;
        }
        new_header.SizeOfRawData = raw_data_size;

        new_header.Characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE; // NOLINT(hicpp-signed-bitwise)

        //print_new_section_header_info(&new_header);

        memcpy(&(((t_pe32 *)pe)->section_header[sections_count]), &new_header, sizeof(IMAGE_SECTION_HEADER));
    }
    else {
        new_header.VirtualAddress = ((t_pe64 *)pe)->pe_header->OptionalHeader.SizeOfImage;
        new_header.PointerToRawData =   ((t_pe64 *)pe)->section_header[sections_count - 1].PointerToRawData +
                                        ((t_pe64 *)pe)->section_header[sections_count - 1].SizeOfRawData;

        new_header.Misc.VirtualSize = packer_config.loader_size;

        // TO SEE
        size_t raw_data_size = 0;
        while(raw_data_size < packer_config.loader_size) {
            raw_data_size += ((t_pe64 *)pe)->pe_header->OptionalHeader.FileAlignment;
        }
        new_header.SizeOfRawData = raw_data_size;

        new_header.Characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE; // NOLINT(hicpp-signed-bitwise)

        //print_new_section_header_info(&new_header);

        memcpy(&(((t_pe64 *)pe)->section_header[sections_count]), &new_header, sizeof(IMAGE_SECTION_HEADER));
    }

    return 1;
}

int set_new_pe_header_values(t_pe *pe) {
    if(pe->s_type == PE32) {
        int old_sections_count = ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections;
        ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections += 1;

        ((t_pe32 *)pe)->pe_header->OptionalHeader.SizeOfCode += ((t_pe32 *)pe)->section_header[old_sections_count - 1].SizeOfRawData;
        ((t_pe32 *)pe)->pe_header->OptionalHeader.SizeOfHeaders += sizeof(IMAGE_SECTION_HEADER);
        ((t_pe32 *)pe)->pe_header->OptionalHeader.SizeOfImage += packer_config.loader_size;
    }
    else {
        int old_sections_count = ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections;
        ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections += 1;

        ((t_pe64 *)pe)->pe_header->OptionalHeader.SizeOfCode += ((t_pe64 *)pe)->section_header[old_sections_count - 1].SizeOfRawData;
        ((t_pe64 *)pe)->pe_header->OptionalHeader.SizeOfHeaders += sizeof(IMAGE_SECTION_HEADER);
        ((t_pe64 *)pe)->pe_header->OptionalHeader.SizeOfImage += packer_config.loader_size;
    }

    return 1;
}

int add_new_pe_section_data(t_pe *pe) {
    char *loader;

    if(pe->s_type == PE32) {
        int sections_count = ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections;
        char **new_section_data = realloc(((t_pe32 *)pe)->section_data, sizeof(char *) * sections_count);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_pe32 *)pe)->section_data = new_section_data;

        // For ASM
        loader_offset32 = ((t_pe32 *)pe)->section_header[sections_count - 1].VirtualAddress;

        loader = patch_loader();
        if (loader == NULL) {
            free(loader);
            log_error("Error during loader patching");
            return -1;
        }
        char *new_section = malloc(((t_pe32 *)pe)->section_header[sections_count - 1].SizeOfRawData);
        if(new_section == NULL) {
            free(loader);
            free(new_section);
            log_error("malloc() failure");
            return -1;
        }
        memset(new_section, 0, ((t_pe32 *)pe)->section_header[sections_count - 1].SizeOfRawData);
        memcpy(new_section, loader, packer_config.loader_size);
        ((t_pe32 *)pe)->section_data[sections_count - 1] = new_section;
    }
    else {
        int sections_count = ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections;
        char **new_section_data = realloc(((t_pe64 *)pe)->section_data, sizeof(char *) * sections_count);
        if (new_section_data == NULL) {
            log_error("realloc() failure");
            return -1;
        }
        ((t_pe64 *)pe)->section_data = new_section_data;

        // For ASM
        loader_offset64 = ((t_pe64 *)pe)->section_header[sections_count - 1].VirtualAddress;

        loader = patch_loader();
        if (loader == NULL) {
            free(loader);
            log_error("Error during loader patching");
            return -1;
        }
        char *new_section = malloc(((t_pe64 *)pe)->section_header[sections_count - 1].SizeOfRawData);
        if(new_section == NULL) {
            free(loader);
            free(new_section);
            log_error("malloc() failure");
            return -1;
        }
        memset(new_section, 0, ((t_pe64 *)pe)->section_header[sections_count - 1].SizeOfRawData);
        memcpy(new_section, loader, packer_config.loader_size);
        ((t_pe64 *)pe)->section_data[sections_count - 1] = new_section;
    }
    free(loader);

    return 1;
}

int pe_insert_section(t_pe *pe) {
    int text_section_index = find_pe_text_section(pe);
    if(text_section_index == -1) {
        log_error("Couldn't find .text section");
        return -1;
    }
    add_pe_section_permission(pe, text_section_index, IMAGE_SCN_MEM_WRITE);

    log_verbose("Creating new section ...");

    if(add_new_pe_section_header(pe) == -1) {
        log_error("Error during new Section Header insertion");
        return -1;
    }

    log_verbose("Setting new sections headers values ...");

    if(set_new_pe_header_values(pe) == -1) {
        log_error("Couldn't set new PE Header values");
        return -1;
    }

    log_verbose("Inserting the section data ...");

    if(add_new_pe_section_data(pe) == -1) {
        log_error("Error during new Section Data insertion");
        return -1;
    }

    log_verbose("Setting new PE entry point ...");

    if(pe->s_type == PE32)
        set_new_pe_entry_to_section(pe, ((t_pe32 *)pe)->pe_header->FileHeader.NumberOfSections-1);
    else
        set_new_pe_entry_to_section(pe, ((t_pe64 *)pe)->pe_header->FileHeader.NumberOfSections-1);

    return 1;
}
#pragma clang diagnostic pop