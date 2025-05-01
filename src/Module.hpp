#ifndef MODULE_HPP
#define MODULE_HPP

#include <string>

int load_module(std::string & filePath, std::string &jsonPath);
void calculate_section_hash(HANDLE &pMap, DWORD &fileSize, std::string &jsonPath);
void calculate_section_chunks_hash(void* pMap, DWORD fileSize, int chunkSize, std::string &jsonPath);

#endif