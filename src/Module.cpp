#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdint>

#include "../include/json/single_include/nlohmann/json.hpp"

#include "Hash.hpp"
#include "Module.hpp"

int load_module(std::string & filePath, std::string &jsonPath){
    std::cout << "Loading module: " << filePath << std::endl;

    HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return 0;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0){
        std::cerr << "Invalid file size." << std::endl;
        CloseHandle(hFile);
        return 0;
    }

    HANDLE hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
    if (!hMap){
        std::cerr << "Failed to create file mapping." << std::endl;
        CloseHandle(hFile);
        return 0;
    }

    HANDLE pMap = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, fileSize);
    if (!pMap){
        std::cerr << "Failed to map view of file." << std::endl;
        CloseHandle(hMap);
        CloseHandle(hFile);
        return 0;
    }

    std::cout << "Calculating Section Hashing..." << std::endl;
    calculate_section_hash(pMap, fileSize, jsonPath);

    // std::cout << "Hashing sections in chunks..." << std::endl;  (FUZZING HASHING may work better) with (sliding window alogrithm)
    // int chunkSize = 0x1000; // 4KB chunks
    // calculate_section_chunks_hash(pMap, fileSize, chunkSize, jsonPath);


    UnmapViewOfFile(pMap);
    CloseHandle(hMap);
    CloseHandle(hFile);

    return 0;
}

void calculate_section_hash(HANDLE &pMap, DWORD &fileSize, std::string &jsonPath){
    PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pMap); // DOS header contains the information about the PE file format. (want to calculate of DOS header?)
    PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<BYTE*>(pMap) + dosHeader->e_lfanew); // NT header contains the information about the PE file format. (want to calculate of NT header?)
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

    nlohmann::json jsonObject;

    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i) {
        DWORD offset = section[i].PointerToRawData;
        DWORD size = section[i].SizeOfRawData;

        std::string sectionName(reinterpret_cast<const char*>(section[i].Name), strnlen(reinterpret_cast<const char*>(section[i].Name), IMAGE_SIZEOF_SHORT_NAME));
        size_t sectionSize = section[i].SizeOfRawData;

        if (offset + size > fileSize){
            std::cerr << "Section " << sectionName << " exceeds file bounds. Skipping." << std::endl;
            continue;
        }

        const void* sectionData = reinterpret_cast<BYTE*>(pMap) + offset;

        std::string blake2HashString = generate_blake2_hash(sectionData, size);
        std::string blake2HexString = get_hex_string(blake2HashString);

        std::string ssdeepHashString = generate_ssdeep_hash(sectionData, size);
        std::string ssdeepHexString = get_hex_string(ssdeepHashString);

        std::cout << "Section " << sectionName << " (blake2) hash: " << blake2HexString << std::endl;
        std::cout << "Section " << sectionName << " (ssdeep) hash: " << ssdeepHashString << std::endl;
        jsonObject[std::string(reinterpret_cast<const char*>(section[i].Name))] = {
            {"blake2", blake2HexString},
            {"ssdeep", ssdeepHashString}
        };
    }

    std::ofstream jsonFile(jsonPath);
    if(jsonFile.is_open()) {
        jsonFile << jsonObject.dump(4);
        jsonFile.close();
    } else {
        std::cerr << "Failed to open JSON file for writing." << std::endl;
    }
    std::cout << "Hashing completed. Hashes saved to hash.json." << std::endl;

    return;
}

void calculate_section_chunks_hash(void* pMap, DWORD fileSize, int chunkSize, std::string &jsonPath){
    auto* dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pMap);
    auto* ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((BYTE*)pMap + dosHeader->e_lfanew);
    auto* section = IMAGE_FIRST_SECTION(ntHeaders);

    nlohmann::json jsonObject;

    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i){
        DWORD offset = section[i].PointerToRawData;
        DWORD size = section[i].SizeOfRawData;

        std::string sectionName(reinterpret_cast<const char*>(section[i].Name), strnlen(reinterpret_cast<const char*>(section[i].Name), IMAGE_SIZEOF_SHORT_NAME));

        if (offset + size > fileSize){
            std::cerr << "Section " << sectionName << " out of file bounds. Skipping." << std::endl;
            continue;
        }

        const BYTE* sectionData = reinterpret_cast<BYTE*>(pMap) + offset;

        std::cout << "Section " << sectionName << " chunked hash: " << std::endl;
        for (size_t j = 0; j < size; j += chunkSize){
            size_t currentChunkSize = std::min<size_t>(chunkSize, size - j);
            std::string chunkHash = generate_blake2_hash(sectionData + j, currentChunkSize);
            std::cout << sectionName << " " << get_hex_string(chunkHash) << std::endl;

            jsonObject[sectionName + "_" + std::to_string(j / chunkSize)] = get_hex_string(chunkHash);
        }
    }

    std::ofstream jsonFile(jsonPath);
    if(jsonFile.is_open()) {
        jsonFile << jsonObject.dump(4);
        jsonFile.close();
    } else {
        std::cerr << "Failed to open JSON file for writing." << std::endl;
    }
    std::cout << "Chunked hashing completed. Hashes saved to chunked_hash.json." << std::endl;
    return;
}