### Basic cmake setup ###

cmake_minimum_required(VERSION 3.2.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS "-Wextra")

project(Silent_Crypt)

# https://metricpanda.com/using-nasm-with-cmake-and-clang/
enable_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER_LOADED)
    set(CAN_USE_ASSEMBLER TRUE)
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

set(ELF_SOURCES src/ELF/elf_allocation.c src/ELF/elf_functions.c src/ELF/pack_elf.c src/ELF/encrypt_elf.c src/ELF/section_insertion.c src/ELF/write_elf.c src/ELF/silvio_infection.c src/ELF/code_cave.c)
set(PE_SOURCES src/PE/pe_allocation.c src/PE/pe_functions.c src/PE/write_pe.c src/PE/pack_pe.c)

set(ASSEMBLY_SOURCES src/common/loader.asm)

set(COMMON_SOURCES ${ELF_SOURCES} ${PE_SOURCES} src/common/file_functions.c src/common/cipher_functions.c src/common/packing_method.c)

set(SOURCES ${SOURCES} ${COMMON_SOURCES} ${ASSEMBLY_SOURCES})

add_executable(Silent_Crypt ${SOURCES})
install(TARGETS Silent_Crypt DESTINATION bin)

### Libs ###

### Static common libs ###
set(ARGTABLE_SOURCES lib/src/argtable3.c)
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})

target_link_libraries(Silent_Crypt argtable3 pthread m)
