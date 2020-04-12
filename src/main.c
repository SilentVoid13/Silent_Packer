#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "elf_packing.h"
#include "pe_packing.h"
#include "file_functions.h"
#include "packer_config.h"

#include "argtable3.h"
#include "log.h"

struct arg_lit *verb, *help, *version, *debug;
struct arg_str *cipher, *packing_method;
struct arg_file *output, *file;
struct arg_end *end;

void display_argtable_help(char *progname, void **argtable) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    exit(0);
}

int main(int argc, char** argv) {
        void *argtable[] = {
                help     = arg_litn("h", "help", 0, 1, "Display this help and exit"),
                version  = arg_litn("V", "version", 0, 1, "Display version info and exit"),
                verb     = arg_litn("v", "verbose", 0, 1, "Verbose output"),
                debug    = arg_litn("d", "debug", 0, 1, "Debug mode"),
                file     = arg_filen("f", "file", "file", 0, 1, "File to pack"),
                cipher   = arg_strn("c", "cipher", "<xor, aes128_ecb>", 0, 1, "Cipher method to use"),
                packing_method = arg_strn("m", "method", "<section_insertion, code_cave, silvio_infection>", 0, 1, "Method to pack the binary"),
                output   = arg_filen("o", "output", "file", 0, 1, "Output file"),
                end      = arg_end(20),
        };

        char *progname = "Silent_Packer";
        char *progversion = "0.1";
        char *progdate = "2020";
        char *author_name = "SilentVoid";

        output->filename[0] = NULL;
        file->filename[0] = NULL;
        cipher->sval[0] = NULL;
        packing_method->sval[0] = NULL;

        int nerrors;
        nerrors = arg_parse(argc, argv, argtable);

        verbose = verb->count;

        if(help->count > 0) {
            display_argtable_help(progname, argtable);
        }

        if(version->count > 0) {
                printf("%s %s\n", progname, progversion);
                printf("Copyright (C) %s %s\n", progdate, author_name);
                exit(0);
        }

        if(nerrors > 0) {
                arg_print_errors(stdout, end, progname);
                printf("Try '%s --help' for more informations.\n", progname);
                exit(1);
        }

        if(file->count > 0 && cipher->count > 0 && packing_method->count > 0) {
            if(
                    (strcmp(cipher->sval[0], "xor") != 0) &&
                    (strcmp(cipher->sval[0], "aes128_ecb") != 0)
            ) {
                log_error("Wrong cipher method");
                display_argtable_help(progname, argtable);
            }

            if(
                    (strcmp(packing_method->sval[0], "silvio_infection") != 0) &&
                    (strcmp(packing_method->sval[0], "code_cave") != 0) &&
                    (strcmp(packing_method->sval[0], "section_insertion") != 0)
            ) {
                log_error("Wrong packing method");
                display_argtable_help(progname, argtable);
            }

            void *file_data;
            size_t file_data_size;

            log_info("Allocating file in memory ...");
            if(allocate_file((char *) file->filename[0], &file_data, &file_data_size) == -1) {
                log_error("Error during file allocation");
                return -1;
            }

            int file_type = check_magic_bytes(file_data, file_data_size);
            if(file_type == ELF_FILE) {
                int arch = get_elf_arch(file_data, file_data_size);
                if(arch == UNKNOWN_ARCH) {
                    munmap(file_data, file_data_size);
                    log_error("Couldn't detect the architecture of the file");
                    return -1;
                }

                if(fill_packer_config((char *) packing_method->sval[0], (char *) cipher->sval[0], arch, file_type, debug->count) == -1) {
                    munmap(file_data, file_data_size);
                    log_error("Error during packer configuration");
                    return -1;
                }

                int p_status = pack_elf((char *) file->filename[0], file_data, file_data_size, (char *) output->filename[0]);
                if(p_status == -1) {
                    log_error("An error occured during the ELF packing");
                }
            }
            else if(file_type == PE_FILE) {
                int arch = get_pe_arch(file_data, file_data_size);
                if(arch == UNKNOWN_ARCH) {
                    munmap(file_data, file_data_size);
                    log_error("Couldn't detect the architecture of the file");
                    return -1;
                }

                if(strcmp(packing_method->sval[0], "silvio_infection") == 0) {
                    munmap(file_data, file_data_size);
                    log_error("Silvio infection is not available for PE files");
                    return -1;
                }

                if(fill_packer_config((char *) packing_method->sval[0], (char *) cipher->sval[0], arch, file_type, debug->count) == -1) {
                    munmap(file_data, file_data_size);
                    log_error("Error during packer configuration");
                    return -1;
                }

                int p_status = pack_pe((char *) file->filename[0], file_data, file_data_size, (char *) output->filename[0]);
                if(p_status == -1) {
                    log_error("An error occured during the PE packing");
                }
             }
            else {
                log_error("Invalid file type");
            }
        }
        else {
            display_argtable_help(progname, argtable);
        }

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 0;
}
