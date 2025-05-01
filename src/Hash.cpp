#include <iostream>
#include <sstream>
#include <iomanip>

#include "../include/BLAKE2/ref/blake2.h"
#include "../include/ssdeep/fuzzy.h"

std::string generate_blake2_hash(const void* data, size_t length){
    unsigned char hash[BLAKE2B_OUTBYTES]; // created a unsigned char array of (64) length
    blake2b(hash, sizeof(hash), data, length, nullptr, 0); // storing the hash of data (anything) in the unsigned char array hash
    return std::string(reinterpret_cast<char*>(hash), BLAKE2B_OUTBYTES);  // converting the hash to a string and returning it
}

std::string get_hex_string(const std::string &hash) {
    std::ostringstream hexStream;
    
    hexStream << std::hex << std::setfill('0');

    for (unsigned char c : hash) {
        hexStream << std::setw(2) << static_cast<int>(c);
    }

    return hexStream.str();
}

std::string generate_ssdeep_hash(const void* data, size_t length) {
    char result[FUZZY_MAX_RESULT];
    int ret = fuzzy_hash_buf(reinterpret_cast<const unsigned char*>(data), length, result);
    if (ret == 0) {
        return std::string(result);
    } else {
        return "ERROR";
    }
}
