#ifndef HASH_HPP
#define HASH_HPP

#include <string>

std::string generate_blake2_hash(const void *data, size_t length);
std::string get_hex_string(const std::string &hash);
std::string generate_ssdeep_hash(const void* data, size_t length);

#endif