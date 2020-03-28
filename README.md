# Silent_Crypt

Silent_Crypt is an ELF / PE packer written in pure C.

This program can be used to obfuscate a binary.

## License

Silent_Crypt is licensed under the GNU AGPLv3 license. Refer to [LICENSE](https://github.com/SilentVoid13/Silent_Crypt/blob/master/LICENSE.txt) for more informations.

## What is a packer ?

> Binary packers alter the original binary data, and restore it (more or less) before execution.

Silent_Crypt is a very basic and common packer, encrypting the code section from the binary (`.text`) and decrypting it before executing it. This technique can be used to obfuscate the binary code to bypass antivirus or to make reverse engineering harder. Packers can also be used to compress a binary to reduce its size.

You can find a good stackoverflow post [here](https://reverseengineering.stackexchange.com/questions/1779/what-are-the-different-types-of-packers).

## Cipher methods

Silent_Crypt currently supports the following cipher methods :

- XOR
- AES

## Encryption methods

Silent_Crypt currently supports the following encryption methods :

- Section Insertion
- Code Cave
- Silvio Text Infection (only for ELF files)

## Usage

```bash
Usage: Silent_Crypt [-hVv] [-f file] [-c <xor>] [-m <section_insertion, code_cave, silvio_infection>] [-o file]
  -h, --help                Display this help and exit
  -V, --version             Display version info and exit
  -v, --verbose             Verbose output
  -f, --file=file           File to pack
  -c, --cipher=<xor>        Cipher method to use
  -m, --method=<section_insertion, code_cave, silvio_infection> Method to pack the binary
  -o, --output=file         Output file
```

## Building

You can either get the compiled version from the latest [release](https://github.com/SilentVoid13/Silent_Crypt/releases), from the [bin](https://github.com/SilentVoid13/Silent_Crypt/tree/master/bin) directory, or compile it yourself.

To build it yourself you will need `cmake`

No windows version is available at the moment.

**On Linux**

You will need `gcc` :

```bash
mkdir build && cd build
cmake ..
make
make install
```

Compiled binaries will be available in the **bin** directory.

## Known_bugs

.

## Contributing

Feel free to contribute. See the [TODO](https://github.com/SilentVoid13/Silent_Crypt/blob/master/TODO.md) list.

## Disclaimer

I am not responsible for what you do with the information and code  provided. This is intended for professional or educational purposes  only.