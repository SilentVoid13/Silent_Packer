//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pe_packing.h"
#include "pe_allocation.h"
#include "file_functions.h"
#include "packer_config.h"
#include "pe_writing.h"
#include "pe_packing_method.h"
#include "pe_encryption.h"

#include "log.h"

int pack_pe(char *file, char *file_data, size_t file_data_size, char *output) {
    log_verbose("Detected arch : x%d", packer_config.arch);
    log_info("Allocating PE in memory ...");

    t_pe *pe = NULL;
    if (allocate_pe(&pe, file_data, file_data_size, packer_config.arch) == -1) {
        log_error("Error during PE allocation");
        return -1;
    }

    // De-allocate mapped file as we don't need it anymore
    munmap(file_data, file_data_size);

    log_info("Encrypting .text section ...");
    if(encrypt_pe(pe) == -1) {
        log_error("Error during PE encryption");
        return -1;
    }

    log_info("Packing using specified method ...");
    if(pe_pack_using_method(pe) == -1) {
        log_error("Error during PE packing");
        return -1;
    }

    char *filename;
    if(output != NULL)
        filename = output;
    else
        filename = "packed.exe";

    log_info("Writing Packed PE to file ...");
    if(write_pe(pe, filename) == -1) {
        log_error("Error during new PE writing");
        return -1;
    }

    log_success("File %s packed into %s !", file, filename);

    return 1;
}

