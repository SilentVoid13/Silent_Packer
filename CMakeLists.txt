### Basic cmake setup ###

cmake_minimum_required(VERSION 3.2.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS "-Wextra")

project(Silent_Crypt)

### Includes ###
include_directories(includes)
include_directories(src/utilities/Silent_Log)

### Sources ###
set(SOURCES src/main.c src/packer.c)

set(UTILITIES_SOURCES src/utilities/Silent_Log/log.c)
set(SOURCES ${SOURCES} ${UTILITIES_SOURCES})

set(LINUX_SOURCES src/linux/elf_allocation.c src/linux/cipher_functions.c)

set(SOURCES ${SOURCES} ${LINUX_SOURCES})

add_executable(Silent_Crypt ${SOURCES})
install(TARGETS Silent_Crypt DESTINATION bin)

### Libs ###

### Static common libs ###
set(ARGTABLE_SOURCES lib/src/argtable3.c)
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})

target_link_libraries(Silent_Crypt argtable3 pthread m)
