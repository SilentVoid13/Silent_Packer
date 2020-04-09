
# Packer
- [x] Make README file
- [ ] Add dynamic library support
- [x] Add PIE support
    - Probably my nasm loader that needs to be changed
- [ ] Add new ciphers
    - [ ] AES
- [x] Add PE Support
- [ ] Clean code
- [ ] Add mprotect in loader
- [x] Add functions prototypes
- [ ] Add malloc variable free
- [x] Add file recognition (ELF or PE)
- [x] Add file arch recognition (x32 or x64) 
- [ ] Free malloc variables
- [x] Add submodule warning in makefile
- [ ] Add possibility to add a custom stub (no encryption just executing stub before jumping to main)

## ELF

### 32 bit
- [x] Add 32 bit support

### Section insertion
- [x] Add section name to the section string table
- [x] Fix string table errors

### Silvio infection
- [ ] Add possibility to add a big payload and add multiple PAGE_SIZE padding

### Code cave
- [x] Add code_cave support

### Heap allocated variables
- [x] file_data mmap - munmap

s_elf struct :

- [ ] t_elf (elf_allocation)
- [ ] elf_header (elf_allocation)
- [ ] prog_header (elf_allocation)
- [ ] section_header (elf_allocation)
- [ ] section_data\[i\]  (elf_allocation)
- [ ] section_data\[i\]\[i\] (elf_allocation)

- [ ] loader (loader_functions)

## PE

### 32 bit
- [x] Add 32 bit support
    - Need to test it

### Code cave
- [ ] Clean code

### Section insertion
- [x] Add section insertion packing method
- [ ] Fix section insertion not working for every binary
    - Don't know why, but it's adding some null bytes at the beginning of the section making the offsets wrong when decrypting the .text

### Heap allocated variables

- [ ] t_pe64 (pe_allocation)
- [ ] dos_header (pe_allocation)
- [ ] dos_stub (pe_allocation)
- [ ] pe_header (pe_allocation)
- [ ] section_header (pe_allocation)
- [ ] section_data\[i\] (pe_allocation)
- [ ] section_data\[i\]\[i\] (pe_allocation)
- [ ] loader (loader_functions)
