
# Packer
- [ ] Make README file
- [ ] Add dynamic library support
- [x] Add PIE support
    - Probably my nasm loader that needs to be changed
- [ ] Add new ciphers
    - [ ] AES
- [ ] Add PE Support
- [ ] Clean code
- [ ] Add functions prototypes
- [ ] Add malloc variable free

## Section insertion
- [x] Add section name to the section string table
- [x] Fix string table errors

## Silvio infection
- [ ] Add possibility to add big payload and add multiple PAGE_SIZE padding

## Code cave
- [x] Add code_cave support


# Heap allocated variables
- [x] file_data mmap - munmap

s_elf struct :

- [ ] t_elf (elf_allocation)
- [ ] elf_header (elf_allocation)
- [ ] prog_header (elf_allocation)
- [ ] section_header (elf_allocation)
- [ ] section_data[i]  (elf_allocation)
- [ ] section_data[i][i] (elf_allocation)

- [ ] loader (elf_functions)
