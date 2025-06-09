//
// Created by eddie phelan on 08/06/2025.
//
#include "Crypto.h"
#include <argon2.h>

std::vector<u_int8_t> Crypto::derive_master_key(const std::string &password, const std::string &salt) {
    constexpr size_t KEY_LEN = 32;
    std::vector<uint8_t> master_key(KEY_LEN);
    auto salt_bytes = reinterpret_cast<const uint8_t*>(salt.data());

    int result = argon2id_hash_raw(
    10,
    1 << 20,
    8,
    password.data(),
    password.size(),
    salt_bytes,
    salt.size(),
    master_key.data(),
    KEY_LEN
    );

    if (result != ARGON2_OK) {
        throw std::runtime_error("Argon2 key derivation failed: " + std::string(argon2_error_message(result)));
    }

    return master_key;
}


