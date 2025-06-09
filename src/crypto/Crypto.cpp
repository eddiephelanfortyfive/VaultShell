//
// Created by eddie phelan on 08/06/2025.
//
#include <argon2.h>
#include <vector>
#include <openssl/rand.h>
#include "crypto/Crypto.h"
#include "utils/Base64.h"

constexpr size_t NONCE_LEN = 12;
constexpr size_t KEY_LEN = 32;
constexpr size_t SALT_LEN = 32;


std::vector<uint8_t> Crypto::derive_master_key(const std::string &password, const std::string &salt) {
    std::vector<uint8_t> master_key(KEY_LEN);
    const auto salt_bytes = reinterpret_cast<const uint8_t*>(salt.data());

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

std::string Crypto::generate_salt() {
    std::vector<uint8_t> salt_bytes(SALT_LEN);

    if (RAND_bytes(salt_bytes.data(), SALT_LEN) != 1) {
        throw std::runtime_error("Failed to generate salt.");
    }

    return Base64::encode(salt_bytes);
}

std::vector<uint8_t> Crypto::generate_nonce() {
    std::vector<uint8_t> nonce(NONCE_LEN);
    if (RAND_bytes(nonce.data(), NONCE_LEN) != 1) {
        throw std::runtime_error("Nonce generation failed");
    }
    return nonce;
}

std::vector<uint8_t> Crypto::generate_kek() {
    std::vector<uint8_t> kek(KEY_LEN);

    if (RAND_bytes(kek.data(), KEY_LEN) != 1) {
        throw std::runtime_error("Failed to generate KEK");
    }

    return kek;
}


bool Crypto::encrypt_kek_with_master(const std::string& master_key_base64, std::string& out_enc_kek, std::string& out_nonce) {
    auto master_key = Base64::decode(master_key_base64); // Decode master key to bytes

    std::vector<uint8_t> kek = generate_kek();
    std::vector<uint8_t> nonce = generate_nonce();
    std::vector<uint8_t> ciphertext, tag;
    if (!aes_gcm_encrypt(master_key, nonce, kek, {}, ciphertext, tag)) {
        return false;
    }

    ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());

    out_enc_kek = Base64::encode(ciphertext);
    out_nonce = Base64::encode(nonce);

    return true;
}

bool Crypto::decrypt_kek_with_master(const std::string& master_key_base64, const std::string& enc_kek_base64, const std::string& nonce_base64) {
    auto master_key = Base64::decode(master_key_base64);
    auto nonce = Base64::decode(nonce_base64);
    auto enc_kek_tag = Base64::decode(enc_kek_base64);

    if (enc_kek_tag.size() < 16) return false;

    std::vector<uint8_t> ciphertext(enc_kek_tag.begin(), enc_kek_tag.end() - 16);
    std::vector<uint8_t> tag(enc_kek_tag.end() - 16, enc_kek_tag.end());

    std::vector<uint8_t> decrypted_kek;

    if (!aes_gcm_decrypt(master_key, nonce, ciphertext, {}, tag, decrypted_kek)) {
        return false;
    }

    return true;
}

bool Crypto::aes_gcm_encrypt(const std::vector<uint8_t>& key,
                             const std::vector<uint8_t>& nonce,
                             const std::vector<uint8_t>& plaintext,
                             const std::vector<uint8_t>& aad,
                             std::vector<uint8_t>& ciphertext,
                             std::vector<uint8_t>& tag) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr))
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, nonce.size(), nullptr);
    EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data());

    int len;
    if (!aad.empty())
        EVP_EncryptUpdate(ctx, nullptr, &len, aad.data(), aad.size());

    ciphertext.resize(plaintext.size());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());

    int ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    tag.resize(16);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data());

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return true;
}

bool Crypto::aes_gcm_decrypt(const std::vector<uint8_t>& key,
                             const std::vector<uint8_t>& nonce,
                             const std::vector<uint8_t>& ciphertext,
                             const std::vector<uint8_t>& aad,
                             const std::vector<uint8_t>& tag,
                             std::vector<uint8_t>& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr))
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, nonce.size(), nullptr);
    EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data());

    int len;
    if (!aad.empty())
        EVP_DecryptUpdate(ctx, nullptr, &len, aad.data(), aad.size());

    plaintext.resize(ciphertext.size());
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag.size(), const_cast<uint8_t*>(tag.data()));

    int ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);

    return ret > 0;
}

bool Crypto::encrypt_with_kek(
    const std::vector<uint8_t>& kek,
    const std::string& plaintext,
    std::string& out_enc_base64,
    std::string& out_nonce_base64)
{
    std::vector<uint8_t> nonce = generate_nonce();

    std::vector<uint8_t> plaintext_bytes(plaintext.begin(), plaintext.end());

    std::vector<uint8_t> aad;

    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> tag;

    bool success = aes_gcm_encrypt(kek, nonce, plaintext_bytes, aad, ciphertext, tag);
    if (!success) {
        return false;
    }

    ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());

    out_enc_base64 = Base64::encode(ciphertext);
    out_nonce_base64 = Base64::encode(nonce);

    return true;
}

bool Crypto::decrypt_with_kek(
    const std::vector<uint8_t>& kek,
    const std::string& enc_base64,
    const std::string& nonce_base64,
    std::string& out_plaintext)
{
    std::vector<uint8_t> ciphertext_tag = Base64::decode(enc_base64);
    std::vector<uint8_t> nonce = Base64::decode(nonce_base64);

    if (ciphertext_tag.size() < 16) {
        return false;
    }

    std::vector<uint8_t> ciphertext(ciphertext_tag.begin(), ciphertext_tag.end() - 16);
    std::vector<uint8_t> tag(ciphertext_tag.end() - 16, ciphertext_tag.end());

    std::vector<uint8_t> aad;

    std::vector<uint8_t> plaintext_bytes;

    bool success = aes_gcm_decrypt(kek, nonce, ciphertext, aad, tag, plaintext_bytes);
    if (!success) {
        return false;
    }

    out_plaintext.assign(plaintext_bytes.begin(), plaintext_bytes.end());
    return true;
}



