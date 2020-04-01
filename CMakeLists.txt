### Basic cmake setup ###

cmake_minimum_required(VERSION 3.2.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS "-Wextra")

project(testy)

# https://metricpanda.com/using-nasm-with-cmake-and-clang/
enable_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER_LOADED)
    set(CAN_USE_ASSEMBLER TRUE)

    # https://stackoverflow.com/questions/49131996/compile-asm-and-c-with-asm-for-debugging
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
    set_source_files_properties(src/common/loader32.asm src/common/loader64.asm PROPERTIES COMPILE_FLAGS "-g -Fdwarf")

    file(GLOB_RECURSE ENGINE_ASM_FILES "src/engine/*.asm")
    set(ENGINE_SOURCES ${ENGINE_SOURCES} ${ENGINE_ASM_FILES})
endif(CMAKE_ASM_NASM_COMPILER_LOADED)

### Includes ###
include_directories(includes)
include_directories(includes/ELF)
include_directories(includes/PE)
include_directories(src/utilities/Silent_Log)

### Sources ###
set(SOURCES src/main.c)

set(UTILITIES_SOURCES src/utilities/Silent_Log/log.c)
set(SOURCES ${SOURCES} ${UTILITIES_SOURCES})

set(ELF_SOURCES src/ELF/elf_allocation.c src/ELF/elf_functions.c src/ELF/elf_packing.c src/ELF/elf_encryption.c src/ELF/elf_section_insertion.c src/ELF/elf_writing.c src/ELF/elf_silvio_infection.c src/ELF/elf_code_cave.c)
set(PE_SOURCES src/PE/pe_allocation.c src/PE/pe_functions.c src/PE/pe_writing.c src/PE/pe_packing.c src/PE/pe_code_cave.c src/PE/pe_packing_method.c src/PE/pe_encryption.c src/PE/pe_section_insertion.c)

set(ASSEMBLY_SOURCES src/common/loader64.asm src/common/loader32.asm)

set(COMMON_SOURCES ${ELF_SOURCES} ${PE_SOURCES} src/common/file_functions.c src/common/cipher_functions.c src/ELF/elf_packing_method.c src/common/loader_functions.c)

set(SOURCES ${SOURCES} ${COMMON_SOURCES} ${ASSEMBLY_SOURCES})

add_executable(Silent_Crypt ${SOURCES})
install(TARGETS Silent_Crypt DESTINATION bin)

### Libs ###

### Static common libs ###
set(ARGTABLE_SOURCES lib/src/argtable3.c)
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})

target_link_libraries(Silent_Crypt argtable3 pthread m)
