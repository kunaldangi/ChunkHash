cmake_minimum_required(VERSION 3.10)

project(chunkhash)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    src/main.cpp
    include/BLAKE2/ref/blake2b-ref.c
    include/BLAKE2/ref/blake2s-ref.c
    src/Module.cpp
    src/Hash.cpp
    include/ssdeep/fuzzy.c
)

add_executable(main ${SOURCES})
