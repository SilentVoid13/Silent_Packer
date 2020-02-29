//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "section_insertion.h"
#include "packing_method.h"
#include "cipher_functions.h"
#include "elf_functions.h"

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
int create_new_section(t_elf *elf, int last_pt_load_index, int last_loadable_section_index) {
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

    // Inserting our new loadable section after the last loadable section
    memcpy(new_section_headers + last_loadable_section_index + 1, &new_section, sizeof(Elf64_Shdr));
    new_section_data[last_loadable_section_index + 1] = loader;

    return 1;
}

int set_new_pt_loader_permissions(t_elf *elf) {
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        if(elf->prog_header[i].p_type == PT_LOAD) {
            elf->prog_header[i].p_flags = PF_X | PF_W | PF_R; // NOLINT(hicpp-signed-bitwise)
        }
    }
    return 1;
}

int insert_section(t_elf *elf) {
    int last_pt_load_index = find_last_segment_of_type(elf, PT_LOAD);
    if(last_pt_load_index == -1) {
        log_error("Couldn't find PT_LOAD segment");
        return -1;
    }

    int last_loadable_section_index = find_last_section(elf, last_pt_load_index);
    if(last_loadable_section_index == -1) {
        log_error("Couldn't find the last Section index");
        return -1;
    }

    if(create_new_section(elf, last_pt_load_index, last_loadable_section_index) == -1) {
        log_error("Error during new Section creation");
        return -1;
    }

    // Since we appended a new section
    last_loadable_section_index += 1;

    // Set new segment size with our new section included
    size_t new_segment_size = elf->prog_header[last_pt_load_index].p_memsz + loader_size;
    elf->prog_header[last_pt_load_index].p_memsz = new_segment_size;
    elf->prog_header[last_pt_load_index].p_filesz = new_segment_size;

    // Set all pt_loader permissions on RWX (pretty ugly)
    set_new_pt_loader_permissions(elf);

    // Add our new section as the new elf entry point
    set_new_elf_entry_to_section(elf, last_loadable_section_index);

    // We shift each section offset to make it correct since we moved sections to leave space for our inserted section
    for(int i = last_loadable_section_index; i < elf->elf_header->e_shnum - 1; i++) {
        elf->section_header[i + 1].sh_offset = elf->section_header[i].sh_offset + elf->section_header[i].sh_size;
    }

    // If the section header string table is after our inserted section, we add + 1 to e_shstrndx to correct its index
    if(elf->elf_header->e_shstrndx > last_loadable_section_index) {
        elf->elf_header->e_shstrndx += 1;
    }

    // We change the offset of the start of the section header to be correct
    // We set it at the end of the last section data == start of section headers (c.f elf structure)
    int section_count = elf->elf_header->e_shnum;
    elf->elf_header->e_shoff = elf->section_header[section_count - 1].sh_offset + elf->section_header[section_count - 1].sh_size;

    return 1;
}