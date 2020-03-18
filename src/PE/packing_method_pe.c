//
// Created by silentvoid on 2/28/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "packing_method_pe.h"
#include "pe_allocation.h"
#include "pe_code_cave.h"

#include "log.h"

struct method_config method_config;

int pack_using_method_pe(t_pe *pe, char *packing_method) {
    if(strcmp(packing_method, "section_insertion") == 0) {
        /*
        method_config.method_type = SECTION_INSERTION_METHOD;
        if (insert_section(pe) == -1) {
            log_error("Error during Section insertion");
            return -1;
        }
        */
    }
    else if(strcmp(packing_method, "code_cave") == 0) {
        method_config.method_type = CODE_CAVE_METHOD;
        if(pe_code_cave_injection(pe) == -1) {
            log_error("Error during Code Cave Injection");
            return -1;
        }
     }
    else if(strcmp(packing_method, "silvio_infection") == 0) {
        /*
        method_config.method_type = SILVIO_INFECTION_METHOD;
        if(silvio_infect(pe) == -1) {
            log_error("Error during Silvio infection");
            return -1;
        }
        */
    }

    return 1;
}