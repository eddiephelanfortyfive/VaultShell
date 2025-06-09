//
// Created by eddie phelan on 08/06/2025.
//
#ifndef CRYPTO_H
#include <string>
#include <vector>
#define CRYPTO_H
class Crypto {

public:
    static std::vector<uint8_t> derive_master_key(const std::string& password, const std::string& salt);
    static bool load_or_generate_salt(std::string& salt);
    static bool encrypt_kek_with_master(const std::string& master_key, std::string& out_enc_kek, std::string& out_nonce);
    static bool decrypt_kek_with_master(const std::string& master_key, const std::string& enc_kek, const std::string& nonce);
};
#endif //CRYPTO_H
