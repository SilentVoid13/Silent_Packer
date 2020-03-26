//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "elf_section_insertion.h"
#include "elf_packing_method.h"
#include "elf_functions.h"
#include "loader_functions.h"

#include "log.h"

Elf64_Shdr new_section = {
        .sh_name = 0,
        .sh_type = SHT_PROGBITS,
        .sh_flags = SHF_EXECINSTR | SHF_ALLOC, // NOLINT(hicpp-signed-bitwise)
        .sh_addr = 0,
        .sh_offset = 0,
        .sh_size = 0,
        .sh_link = 0,
        .sh_info = 0,
        .sh_addralign = 16,
        .sh_entsize = 0,
};

int set_new_elf_section_string_table(t_elf *elf) {
    char *new_string_table;

    char *section_name = ".decryption";
    size_t section_name_length = strlen(section_name);

    int section_string_table_index = elf->elf_header->e_shstrndx;

    printf("section_string_table_index: %d\n", section_string_table_index);

    size_t new_string_table_size = elf->section_header[section_string_table_index].sh_size + section_name_length + 1;
    new_string_table = realloc(elf->section_data[section_string_table_index], new_string_table_size);
    if(new_string_table == NULL) {
        log_error("realloc() failure");
        return -1;
    }
    memcpy(new_string_table + elf->section_header[section_string_table_index].sh_size, section_name, section_name_length+1);

    // We set it to the end of the old section_string_table
    new_section.sh_name = elf->section_header[section_string_table_index].sh_size;

    elf->section_data[section_string_table_index] = new_string_table;
    elf->section_header[section_string_table_index].sh_size = new_string_table_size;


    return 1;
}

int set_new_elf_section_symtab_sh_link_value(t_elf *elf) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        char *section_name = find_elf_section_name(elf, i);
        if(strcmp(section_name, ".symtab") == 0) {
            elf->section_header[i].sh_link += 1;
        }
    }

    return 1;
}

/* Map example
     *
     * --------
     * loadable1
     * --------
     * loadable2
     * --------
     * loadable3 --> last_loadable_section_index
     * --------
     * non_loadable1
     * --------
     * non_loadable2
     * --------
     * new_created_empty_section
     * --------
     *
     * We change it to this :
     *
     * --------
     * loadable1
     * --------
     * loadable2
     * --------
     * loadable3 --> last_loadable_section_index
     * --------
     * new_section_loadable4
     * --------
     * non_loadable1
     * --------
     * non_loadable2
     * --------
     *
*/
int elf_section_create_new_section(t_elf *elf, int last_pt_load_index, int last_loadable_section_index) {
    Elf64_Shdr *new_section_headers;
    char **new_section_data;
    char *loader;

    elf->elf_header->e_shnum += 1;

    // We realloc to add the new section header
    size_t new_section_headers_size = sizeof(Elf64_Shdr) * elf->elf_header->e_shnum;
    new_section_headers = realloc(elf->section_header, new_section_headers_size);
    if(new_section_headers == NULL) {
        log_error("realloc() failure");
        return -1;
    }

    // We realloc to add the new section data
    size_t new_section_data_size = sizeof(char *) * elf->elf_header->e_shnum;
    new_section_data = realloc(elf->section_data, new_section_data_size);
    if(new_section_data == NULL) {
        log_error("realloc() failure");
        return -1;
    }

    // Re-map to new memory location
    elf->section_header = new_section_headers;
    elf->section_data = new_section_data;

    // Set new section values
    // sh_offset and sh_addr == end of last loadable segment since we know we will add it there
    new_section.sh_offset = elf->prog_header[last_pt_load_index].p_offset + elf->prog_header[last_pt_load_index].p_memsz;
    new_section.sh_addr = elf->prog_header[last_pt_load_index].p_vaddr + elf->prog_header[last_pt_load_index].p_memsz;

    new_section.sh_size = loader_size;

    // For ASM
    loader_offset = new_section.sh_addr;

    loader = patch_loader();
    if(loader == NULL) {
        log_error("Error during loader patching");
        return -1;
    }

    // -1 because e_shnum starts at 1 / -1 because we created an empty section which doesn't count
    size_t remaining_after_section_headers_data_size = sizeof(Elf64_Shdr) * (elf->elf_header->e_shnum - last_loadable_section_index - 1 - 1);
    size_t remaining_after_section_headers_count = sizeof(char *) * (elf->elf_header->e_shnum - last_loadable_section_index - 1 - 1);

    // We move all sections after the last loadable section to + 1
    memmove(new_section_headers + last_loadable_section_index + 2, new_section_headers + last_loadable_section_index + 1, remaining_after_section_headers_data_size);
    // Shift all char * pointer after the last loadable section to + 1
    memmove(new_section_data + last_loadable_section_index + 2, new_section_data + last_loadable_section_index + 1, remaining_after_section_headers_count);

    // Since we append a new section
    last_loadable_section_index += 1;

    // If the section header string table is after our inserted section, we add + 1 to e_shstrndx to correct its index
    if(elf->elf_header->e_shstrndx > last_loadable_section_index) {
        elf->elf_header->e_shstrndx += 1;
    }

    // We set a new proper string table
    if(set_new_elf_section_string_table(elf) == -1) {
        log_error("Error setting new string table");
        return -1;
    }

    // Fixing sh_link symbol_table index value
    // https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblj/index.html#chapter6-47976
    if(set_new_elf_section_symtab_sh_link_value(elf) == -1) {
        log_error("Error modifying symtab sh_link value");
        return -1;
    }

    // Inserting our new loadable section after the last loadable section
    memcpy(new_section_headers + last_loadable_section_index, &new_section, sizeof(Elf64_Shdr));
    new_section_data[last_loadable_section_index] = loader;

    return 1;
}

/*
void print_link_fields(t_elf *elf) {
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        printf("section_name : %s\n", find_elf_section_name(elf, i));
        printf("index : %d\n", i);
        printf("sh_type : %d\n", elf->section_header[i].sh_type);
        printf("sh_link : %d\n", elf->section_header[i].sh_link);
        printf("\n");
    }
}
*/

int set_new_elf_section_pt_loader_permissions(t_elf *elf) {
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        if(elf->prog_header[i].p_type == PT_LOAD) {
            elf->prog_header[i].p_flags = PF_X | PF_W | PF_R; // NOLINT(hicpp-signed-bitwise)
        }
    }
    return 1;
}

int elf_insert_section(t_elf *elf) {
    int last_pt_load_index = find_last_elf_segment_of_type(elf, PT_LOAD);
    if(last_pt_load_index == -1) {
        log_error("Couldn't find PT_LOAD segment");
        return -1;
    }

    int last_loadable_section_index = find_last_elf_section(elf, last_pt_load_index);
    if(last_loadable_section_index == -1) {
        log_error("Couldn't find the last Section index");
        return -1;
    }

    if(elf_section_create_new_section(elf, last_pt_load_index, last_loadable_section_index) == -1) {
        log_error("Error during new Section creation");
        return -1;
    }

    // Since we appended a new section
    last_loadable_section_index += 1;

    method_config.concerned_section = last_loadable_section_index;


    // Set new segment size with our new section included
    size_t new_segment_size = elf->prog_header[last_pt_load_index].p_memsz + loader_size;
    elf->prog_header[last_pt_load_index].p_memsz = new_segment_size;
    elf->prog_header[last_pt_load_index].p_filesz = new_segment_size;

    // Set all pt_loader permissions on RWX (pretty ugly)
    set_new_elf_section_pt_loader_permissions(elf);

    // Add our new section as the new elf entry point
    set_new_elf_entry_to_section(elf, last_loadable_section_index);

    // We shift each section offset to make it correct since we moved sections to leave space for our inserted section
    for(int i = last_loadable_section_index; i < elf->elf_header->e_shnum - 1; i++) {
        elf->section_header[i + 1].sh_offset = elf->section_header[i].sh_offset + elf->section_header[i].sh_size;
    }

    // We change the offset of the start of the section header to be correct
    // We set it at the end of the last section data == start of section headers (c.f elf structure)
    int section_count = elf->elf_header->e_shnum;
    elf->elf_header->e_shoff = elf->section_header[section_count - 1].sh_offset + elf->section_header[section_count - 1].sh_size;

    return 1;
}