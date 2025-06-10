//
// Created by eddie phelan on 08/06/2025.
//
#pragma once
#ifndef CRYPTO_H
#include <string>
#include <vector>
#define CRYPTO_H
class Crypto {

public:
    static std::vector<uint8_t> derive_master_key(const std::string& password, const std::string& salt);
    static std::string generate_salt();  // Generate and Base64 encode salt
    static std::vector<uint8_t> generate_nonce();

    static bool encrypt_kek_with_master(const std::string& master_key, std::string& out_enc_kek, std::string& out_nonce);
    static bool decrypt_kek_with_master(const std::string& master_key, const std::string& enc_kek, const std::string& nonce);
    static std::vector<uint8_t> generate_kek();

    static bool encrypt_with_kek(
        const std::vector<uint8_t>& kek,
        const std::string& plaintext,
        const std::string& aad,
        std::string& out_enc_base64,
        std::string& out_nonce_base64);

    static bool decrypt_with_kek(
        const std::vector<uint8_t>& kek,
        const std::string& enc_base64,
        const std::string& aad,
        const std::string& nonce_base64,
        std::string& out_plaintext);

    static bool aes_gcm_encrypt(const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& nonce,
                               const std::vector<uint8_t>& plaintext,
                               const std::vector<uint8_t>& aad,
                               std::vector<uint8_t>& ciphertext,
                               std::vector<uint8_t>& tag);

    static bool aes_gcm_decrypt(const std::vector<uint8_t>& key,
                                const std::vector<uint8_t>& nonce,
                                const std::vector<uint8_t>& ciphertext,
                                const std::vector<uint8_t>& aad,
                                const std::vector<uint8_t>& tag,
                                std::vector<uint8_t>& plaintext);
};
#endif //CRYPTO_H
