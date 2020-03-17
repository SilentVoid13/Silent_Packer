//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#include "pack_pe.h"
#include "pe_allocation.h"
#include "file_functions.h"
#include "write_pe.h"

#include "log.h"

int pack_pe(char *file, char *cipher, char *packing_method, char *output) {
    void *file_data;
    size_t file_data_size;

    log_info("Allocating file in memory ...");
    if(allocate_file(file, &file_data, &file_data_size) == -1) {
        log_error("Error during file allocation");
        return -1;
    }

    log_info("Allocating PE in memory ...");
    t_pe *pe = NULL;
    if(allocate_pe(&pe, file_data, file_data_size) == -1) {
        log_error("Error during PE allocation");
        return -1;
    }

    // De-allocate mapped file as we don't need it anymore
    munmap(file_data, file_data_size);

    // TODO: Encrypt + pack stub

    char *filename;
    if(output != NULL)
        filename = output;
    else
        filename = "packed.exe";

    log_info("Writing Packed PE to file ...");
    if(write_pe(pe, filename) == -1) {
        log_error("Error during new ELF writing");
        return -1;
    }

    return 1;
}
