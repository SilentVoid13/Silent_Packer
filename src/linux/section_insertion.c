//
// Created by silentvoid on 2/26/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "section_insertion.h"
#include "cipher_functions.h"

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

size_t loader_size;
size_t infos_size;

int create_new_section(t_elf *elf, int last_pt_load_index, int last_section_index) {
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
    new_section.sh_offset = elf->prog_header[last_pt_load_index].p_offset + elf->prog_header[last_pt_load_index].p_memsz;
    new_section.sh_addr = elf->prog_header[last_pt_load_index].p_vaddr + elf->prog_header[last_pt_load_index].p_memsz;

    // TODO
    new_section.sh_size = loader_size;

    loader = malloc(loader_size);
    if(loader == NULL) {
        log_error("malloc() failure");
        return -1;
    }

    // Copy the loader asm entry point to our section data
    memcpy(loader, loader_entry_point, loader_size);

    printf("key: %d\n", cipher_key);

    // Set globals variables for asm code
    // Variables are 8 bytes 64 bit addresses (uint64_t)
    memcpy(loader + loader_size - CIPHER_KEY_OFFSET, &cipher_key, sizeof(uint64_t));
    memcpy(loader + loader_size - TEXT_ENTRY_POINT_OFFSET, &text_entry_point, sizeof(uint64_t));
    memcpy(loader + loader_size - TEXT_DATA_SIZE_OFFSET, &text_data_size, sizeof(uint64_t));

    size_t a = sizeof(Elf64_Shdr) * (elf->elf_header->e_shnum - last_section_index - 2);
    size_t b = sizeof(char *) * (elf->elf_header->e_shnum - last_section_index - 2);

    // Move the last section header / last section data to the new
    memmove(new_section_headers + last_section_index + 2, new_section_headers + last_section_index + 1, a);
    memmove(new_section_data + last_section_index + 2, new_section_data + last_section_index + 1, b);

    memcpy(new_section_headers + last_section_index + 1, &new_section, sizeof(Elf64_Shdr));
    new_section_data[last_section_index + 1] = loader;

    return 1;
}

int set_new_elf_entry(t_elf *elf, int last_section_index) {
    // TODO
    Elf64_Addr last_entry = elf->elf_header->e_entry;
    elf->elf_header->e_entry = elf->section_header[last_section_index].sh_addr;
    int32_t jump = last_entry - (elf->elf_header->e_entry + loader_size - infos_size);
    memcpy(elf->section_data[last_section_index] + loader_size - (infos_size + 4), &jump, 4);

    return 1;
}

int find_last_program_segment_type(t_elf *elf, unsigned int p_type) {
    int index = -1;
    for(int i = 0; i < elf->elf_header->e_phnum; i++) {
        if(elf->prog_header[i].p_type == p_type) {
            index = i;
        }
    }
    return index;
}

// Find last section in memory
int find_last_section(t_elf *elf, int last_pt_load_index) {
    int index = -1;
    for(int i = 0; i < elf->elf_header->e_shnum; i++) {
        Elf64_Phdr *program_header = elf->prog_header + last_pt_load_index;
        Elf64_Shdr	*section_header = elf->section_header + i;

        if(section_header->sh_addr + section_header->sh_size >= program_header->p_vaddr + program_header->p_memsz) {
            index = i;
        }
    }
    return index;
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
    int last_pt_load_index = find_last_program_segment_type(elf, PT_LOAD);
    if(last_pt_load_index == -1) {
        log_error("Couldn't find PT_LOAD segment");
        return -1;
    }

    int last_section_index = find_last_section(elf, last_pt_load_index);
    if(last_section_index == -1) {
        log_error("Couldn't find the last Section index");
        return -1;
    }

    if(create_new_section(elf, last_pt_load_index, last_section_index) == -1) {
        log_error("Error during new Section creation");
        return -1;
    }

    // Since we appended a new section
    last_section_index += 1;


    // Set new segment size with our new section
    size_t new_segment_size = elf->prog_header[last_pt_load_index].p_memsz + loader_size;
    elf->prog_header[last_pt_load_index].p_memsz = new_segment_size;
    elf->prog_header[last_pt_load_index].p_filesz = new_segment_size;

    // Set all pt_loader permissions on RWX (pretty ugly)
    set_new_pt_loader_permissions(elf);

    // Add our new section as the new elf entry point
    set_new_elf_entry(elf, last_section_index);

    // TODO: Understand this
    for(int i = last_section_index; i < elf->elf_header->e_shnum - 1; i++) {
        elf->section_header[i + 1].sh_offset = elf->section_header[i].sh_offset + elf->section_header[i].sh_size;
    }

    if(elf->elf_header->e_shstrndx > last_section_index) {
        elf->elf_header->e_shstrndx += 1;
    }

    int section_count = elf->elf_header->e_shnum;
    elf->elf_header->e_shoff = elf->section_header[section_count - 1].sh_offset + elf->section_header[section_count - 1].sh_size;

    return 1;
}