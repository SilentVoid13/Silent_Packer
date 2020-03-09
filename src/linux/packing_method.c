//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "packing_method.h"
#include "elf_allocation.h"
#include "packer.h"
#include "section_insertion.h"
#include "silvio_infection.h"
#include "code_cave.h"

#include "log.h"

struct method_config method_config;

int pack_using_method(t_elf *elf, char *packing_method) {

    if(strcmp(packing_method, "section_insertion") == 0) {
        method_config.method_type = SECTION_INSERTION_METHOD;
        if (insert_section(elf) == -1) {
            log_error("Error during Section insertion");
            return -1;
        }
    }
    else if(strcmp(packing_method, "code_cave") == 0) {
        method_config.method_type = CODE_CAVE_METHOD;
        if(code_cave_injection(elf) == -1) {
            log_error("Error during Code Cave Injection");
            return -1;
        }
     }
    else if(strcmp(packing_method, "silvio_infection") == 0) {
        method_config.method_type = SILVIO_INFECTION_METHOD;
        if(silvio_infect(elf) == -1) {
            log_error("Error during Silvio infection");
            return -1;
        }
    }

    return 1;
}