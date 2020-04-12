# Silent_Packer

Silent_Packer is an ELF / PE packer written in pure C.

This program can be used to obfuscate a binary. This packer supports PIE binaries.

My code is surely not perfect, do not hesitate to make pull requests or to contact me for any problems / code enhancement suggestion at [silentvoid13@protonmail.com](mailto:silentvoid13@protonmail.com).

## License

Silent_Packer is licensed under the GNU AGPLv3 license. Refer to [LICENSE](https://github.com/SilentVoid13/Silent_Packer/blob/master/LICENSE.txt) for more informations.

## What is a packer ?

> Binary packers alter the original binary data, and restore it (more or less) before execution.

Silent_Packer is a very basic and common packer, encrypting the code section from the binary (`.text`) and decrypting it before executing it. This technique can be used to obfuscate the binary code to bypass antiviruses or to make reverse engineering harder. Packers can also be used to compress a binary to reduce its size.

You can find a more detailed stackoverflow post [here](https://reverseengineering.stackexchange.com/questions/1779/what-are-the-different-types-of-packers).

## File formats

Silent_Packer currently support the following file formats :

- ELF64
- ELF32
- PE32+
- PE32

I will maybe add the MACH-O file format later. You can suggest me a file format that you want me to add.

## Cipher methods

Silent_Packer currently support the following cipher methods :

- XOR
- AES-128-ECB

Support for some more secure AES modes should arrive soon (AES-128-CBC, AES-256, ...)

You can suggest me a cipher method that you want me to add.

## Encryption methods

Silent_Packer currently support the following encryption methods :

- Section Insertion
- Code Cave
- Silvio Text Infection (only for ELF files)

For a basic explanation on how does these methods work, see the [METHODS](https://github.com/SilentVoid13/Silent_Packer/blob/master/METHODS.md) file.

## Usage

```bash
Usage: Silent_Packer [-hVvd] [-f file] [-c <xor, aes128_ecb>] [-m <section_insertion, code_cave, silvio_infection>] [-o file]
  -h, --help                Display this help and exit
  -V, --version             Display version info and exit
  -v, --verbose             Verbose output
  -d, --debug               Debug mode
  -f, --file=file           File to pack
  -c, --cipher=<xor, aes128_ecb> Cipher method to use
  -m, --method=<section_insertion, code_cave, silvio_infection> Method to pack the binary
  -o, --output=file         Output file
```

## Building

You can either get the compiled version from the latest [release](https://github.com/SilentVoid13/Silent_Packer/releases), from the [bin](https://github.com/SilentVoid13/Silent_Packer/tree/master/bin) directory, or compile it yourself.

This project relies on [Silent_Log](https://github.com/SilentVoid13/Silent_Log) for logging messages.

To build it yourself you will need `cmake`, `gcc` and `nasm`.

No windows version is available at the moment.

**On Linux**

```bash
mkdir build && cd build
cmake ..
make
make install
```

The compiled binary will be available in the **bin** directory.

## Known_bugs

Feel free to report any bugs, so that I can fix them.

The packer has some recurrent bugs with PE files, especially with big binaries with a lot of dynamic libs. The section insertion method has a weird behavior sometimes (adding some null bytes at the beginning of the section making the offsets wrong).

## Contributing

Feel free to contribute. See the [TODO](https://github.com/SilentVoid13/Silent_Packer/blob/master/TODO.md) list.

## Disclaimer

I am not responsible for what you do with the information and code provided. This is intended for professional or educational purposes only.