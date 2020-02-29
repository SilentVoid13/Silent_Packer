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
include_directories(src/utilities/Silent_Log)

### Sources ###
set(SOURCES src/main.c src/packer.c)

set(UTILITIES_SOURCES src/utilities/Silent_Log/log.c)
set(SOURCES ${SOURCES} ${UTILITIES_SOURCES})

set(LINUX_SOURCES src/linux/elf_allocation.c src/linux/cipher_functions.c src/linux/section_insertion.c src/linux/write_elf.c src/linux/packing_method.c src/linux/silvio_infection.c src/linux/elf_functions.c)
set(LINUX_ASSEMBLY_SOURCES src/linux/loader.asm)

set(SOURCES ${SOURCES} ${LINUX_SOURCES} ${LINUX_ASSEMBLY_SOURCES})

add_executable(Silent_Crypt ${SOURCES})
install(TARGETS Silent_Crypt DESTINATION bin)

### Libs ###

### Static common libs ###
set(ARGTABLE_SOURCES lib/src/argtable3.c)
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})

target_link_libraries(Silent_Crypt argtable3 pthread m)
