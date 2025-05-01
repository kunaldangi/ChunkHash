#include <iostream>
#include <iomanip>
#include <windows.h>

#include "Hash.hpp"
#include "Module.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2){
        std::cerr << "Usage: " << argv[0] << " <path_to_file> <json_file>" << std::endl;
        return 0;
    }

    if (argv[1] == nullptr){
        std::cerr << "Invalid file name or path." << std::endl;
        return 0;
    }

    if(argv[2] == nullptr){
        std::cerr << "Invalid json file name or path." << std::endl;
        return 0;
    }

    std::string filePath = argv[1];
    std::cout << "Path: " << filePath << std::endl;

    std::string jsonPath = argv[2];
    std::cout << "Json Path: " << jsonPath << std::endl;

    load_module(filePath, jsonPath);
    return 0;
}