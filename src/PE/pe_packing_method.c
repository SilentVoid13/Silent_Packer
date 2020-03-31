//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_packing_method.h"
#include "pe_allocation.h"
#include "pe_code_cave.h"
#include "pe_section_insertion.h"

#include "log.h"

struct method_config method_config;

int pe_pack_using_method(t_pe *pe, char *packing_method) {
    if(strcmp(packing_method, "section_insertion") == 0) {
        method_config.method_type = SECTION_INSERTION_METHOD;
        if (pe_insert_section(pe) == -1) {
            log_error("Error during Section insertion");
            return -1;
        }
    }
    else if(strcmp(packing_method, "code_cave") == 0) {
        method_config.method_type = CODE_CAVE_METHOD;
        if(pe_code_cave_injection(pe) == -1) {
            log_error("Error during Code Cave Injection");
            return -1;
        }
    }

    return 1;
}