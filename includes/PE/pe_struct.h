//
// Created by silentvoid on 3/16/20.
// Copyright (c) 2020 SilentVoid. All rights reserved.
//

#ifndef SILENT_CRYPT_PE_STRUCT_H
#define SILENT_CRYPT_PE_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

#define DOSMAG 0x5a4d
#define PEMAG 0x00004550

#define IMAGE_FILE_RELOCS_STRIPPED 0x0001 /**< No relocation info. */
#define IMAGE_FILE_EXECUTABLE_IMAGE 0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED 0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED 0x0008
#define IMAGE_FILE_AGGRESIVE_WS_TRIM 0x0010
#define IMAGE_FILE_LARGE_ADDRESS_AWARE 0x0020
#define IMAGE_FILE_16BIT_MACHINE 0x0040
#define IMAGE_FILE_BYTES_REVERSED_LO 0x0080
#define IMAGE_FILE_32BIT_MACHINE 0x0100
#define IMAGE_FILE_DEBUG_STRIPPED 0x0200
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP 0x0400
#define IMAGE_FILE_NET_RUN_FROM_SWAP 0x0800
#define IMAGE_FILE_SYSTEM0x1000
#define IMAGE_FILE_DLL 0x2000
#define IMAGE_FILE_UP_SYSTEM_ONLY 0x4000
#define IMAGE_FILE_BYTES_REVERSED_HI 0x8000

/* These are the settings of the Machine field. */
#define IMAGE_FILE_MACHINE_UNKNOWN 0x0 /**< COFF header's Machine field for
                                         unknown machine. */
#define IMAGE_FILE_MACHINE_I386 0x014c /**< COFF header's Machine field for
                                         I386 machines. */
#define IMAGE_FILE_MACHINE_ARM 0x01c0 /**< COFF header's Machine field for ARM
                                        machines. */
#define IMAGE_FILE_MACHINE_AMD64 0x8664 /**< COFF header's Machine field for
                                          AMD machines. */

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

#define IMAGE_DOS_SIGNATURE 0x5A4D /**< DOS header signature. Equals to 'MZ'. */
#define IMAGE_NT_SIGNATURE 0x00004550 /**< PE header signature. Equals to 'PE\0\0'. */
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x10B /**< OPTIONAL header 32bits signature. */
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20B /**< OPTIONAL header 64bits signature. */

#define IMAGE_SIZEOF_SHORT_NAME 8 /**< Max length for the section header's name field. */

#define IMAGE_SCN_CNT_CODE 0x00000020 /**< The section contains executable code. */
#define IMAGE_SCN_MEM_EXECUTE 0x20000000 /**< The section can be executed as code. */
#define IMAGE_SCN_MEM_READ 0x40000000 /**< The section can be read. */
#define IMAGE_SCN_MEM_WRITE 0x80000000 /**< The section can be written. */

/* PE classes */
#define PECLASSNONE 0 /**< Unknown class. */
#define PECLASS32 1 /**< 32-bit objects. */
#define PECLASS64 2 /**< 64-bit objects. */

typedef struct _IMAGE_DOS_HEADER {
    uint16_t e_magic; /**< DOS header signature. Must be equal to 'MZ' */
    uint16_t e_cblp; /**< Bytes on last page of file */
    uint16_t e_cp; /**< Pages in file */
    uint16_t e_crlc; /**< Relocations */
    uint16_t e_cparhdr; /**< Size of header in paragraphs */
    uint16_t e_minalloc; /**< Minimum extra paragraphs needed */
    uint16_t e_maxalloc; /**< Maximum extra paragraphs needed */
    uint16_t e_ss; /**< Initial (relative) SS value */
    uint16_t e_sp; /**< Initial SP value */
    uint16_t e_csum; /**< Checksum */
    uint16_t e_ip; /**< Initial IP value */
    uint16_t e_cs; /**< Initial (relative) CS value */
    uint16_t e_lfarlc; /**< File address of relocation table */
    uint16_t e_ovno; /**< Overlay number */
    uint16_t e_res[4]; /**< Reserved words */
    uint16_t e_oemid; /**< OEM identifier (for e_oeminfo) */
    uint16_t e_oeminfo; /**< OEM information; e_oemid specific */
    uint16_t e_res2[10]; /**< Reserved words */
    uint32_t e_lfanew; /**< Offset to extended header */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    uint16_t  Machine;
    uint16_t  NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t  SizeOfOptionalHeader;
    uint16_t  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    uint32_t VirtualAddress;
    uint32_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER32 {
    uint16_t Magic; /**< OPTIONAL header signature. Equals to
                      #IMAGE_NT_OPTIONAL_HDR32_MAGIC for 32bits EXE files. */
    uint8_t MajorLinkerVersion; /**< The version of the linker that produced the file. */
    uint8_t MinorLinkerVersion; /**< The version of the linker that produced the file. */
    uint32_t SizeOfCode; /**< The combined and rounded-up size of all the code
                           sections (most of the time, '.text' section). */
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUnitializedData;
    uint32_t AddressOfEntryPoint; /**< The address where the loader will begin
                                    the execution. */
    uint32_t BaseOfCode; /**< Relative virtual address where the code sections
                           begin. */
    uint32_t BaseOfData; /**< Relative virtual address where the data sections
                           begin. */
    uint32_t ImageBase; /**< Assumed location where the file will be
                          memory-mapped. */
    uint32_t SectionAlignment; /**< Alignment value for a section. */
    uint32_t FileAlignment; /**< Alignment value for the file's raw data. */
    uint16_t MajorOperatingSystemVersion; /**< The minimum version of the
                                            operating system required to use
                                            the executable. */
    uint16_t MinorOperatingSystemVersion; /**< The minimum version of the
                                            operating system required to use
                                            the executable. */
    uint16_t MajorImageVersion; /**< User-definable field to allow different
                                  version of an EXE file. */
    uint16_t MinorImageVersion; /**< User-definable field to allow different
                                  version of an EXE file. */
    uint16_t MajorSubsystemVersion; /**< The minimum subsystem version required
                                      to run the EXE file. Typical value 3.10
                                      (meaning Windows NT 3.1). */
    uint16_t MinorSubsystemVersion; /**< The minimum subsystem version required
                                      to run the EXE file. Typical value 3.10
                                      (meaning Windows NT 3.1). */
    uint32_t Win32VersionValue; /**< Reserved. */
    uint32_t SizeOfImage; /**< Total size starting from the image base to the
                            end of the last section. The end of the last
                            section is rounded up to the nearest multiple of
                            the #SectionAlignment value. */
    uint32_t SizeOfHeaders; /**< Size of the PE headers and the section table.
                              The raw section data starts immediately after all
                              the header components. */
    uint32_t CheckSum; /**< CRC checksum. Usualy ignored and set to 0. */
    uint16_t Subsystem; /**< Type of subsystem used for its user interface. */
    uint16_t DllCharacteristics; /**< Set of flags indicating under which
                                   circumstances a DLL's initialization
                                   function will be called. Appears to always
                                   be set to 0. */
    uint32_t SizeOfStackReserve; /**< The amount of virtual memory to reserve
                                   for the initial thread's stack. */
    uint32_t SizeOfStackCommit; /**< The amount of memory initially committed
                                  for the initial thread's stack. */
    uint32_t SizeOfHeapReserve; /**< The amount of virtual memory to reserve
                                  for the initial process heap. */
    uint32_t SizeOfHeapCommit; /**< The amount of memory initially committed in
                                 the process heap. */
    uint32_t LoaderFlags; /**< Debugging related field. */
    uint32_t NumberOfRvaAndSizes; /**< The number of entries in the
                                    #DataDirectory array. Always set to 16 by
                                    the tools. */
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; /**< Array of #IMAGE_DATA_DIRECTORY structures. See WINNT.H. */
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS32 {
    uint32_t Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32;

typedef struct _IMAGE_SECTION_HEADER {
    uint8_t Name[IMAGE_SIZEOF_SHORT_NAME]; /**< 8-bytes ANSI name of the
                                             section. If the length of the
                                             string is greater than 8, it is
                                             truncated without appending a NULL
                                             terminating byte. */
    union {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc; /**< Size of the code or the data before beeing rounded up to the
              nearest file alignment multiple. */
    uint32_t VirtualAddress; /**< Relative virtual address to where the loader
                               should map the section. */
    uint32_t SizeOfRawData; /**< Size of the section after beeing rounded up to
                              the nearest file alignment multiple. */
    uint32_t PointerToRawData; /**< File based offset to the raw data. */
    uint32_t PointerToRelocations; /**< Set to 0 for EXE files. */
    uint32_t PointerToLinenumbers; /**< File based offset to the line number
                                     table. */
    uint16_t NumberOfRelocations; /**< Irrelevant for EXE files. */
    uint16_t NumberOfLinenumbers; /**< Number of line numbers in the line
                                    number table for the section. */
    uint32_t Characteristics; /**< Set of flags describing the section.
                                (Readable, Writeable, Executable, etc.) */
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#endif //SILENT_CRYPT_PE_STRUCT_H
