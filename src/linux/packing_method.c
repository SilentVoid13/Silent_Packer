//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "packing_method.h"
#include "elf_allocation.h"
#include "packer.h"
#include "section_insertion.h"
#include "silvio_infection.h"

#include "log.h"

int pack_using_method(t_elf *elf, char *packing_method) {

    if(strcmp(packing_method, "section_insertion") == 0) {
        if (insert_section(elf) == -1) {
            log_error("Error during Section insertion");
            return -1;
        }
    }
    else if(strcmp(packing_method, "code_cave") == 0) {
        // TODO
    }
    else if(strcmp(packing_method, "silvio_infection") == 0) {
        if(silvio_infect(elf) == -1) {
            log_error("Error during Silvio infection");
            return -1;
        }
    }

    return 1;
}