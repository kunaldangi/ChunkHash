# ChunkHash

ChunkHash is an analyzing and comparison tool for calculating hashes of Windows Portable Executable (PE) file sections using both cryptographic and fuzzy hashing algorithm.

## Features

- Calculate hashes of PE file sections using cryptographic and fuzzy hashing algorithms.
- Export results to JSON file format.
- Compatible with Windows PE (Portable Executable) format (.exe, .dll, etc.).

## Usage

- `chunkhash.exe <PE file> <json file>`

## Building

- Download all submodules

```sh
mkdir build
cd build
cmake .. -G "MingW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake # generate as per your compiler
cmake --build .
```

