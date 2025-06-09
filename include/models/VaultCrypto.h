//
// Created by eddie phelan on 09/06/2025.
//

#ifndef VAULTCRYPTOINFO_H
#define VAULTCRYPTOINFO_H

#include <string>
#include "external/nlohmann/json.hpp"

class VaultCrypto {
public:
    std::string salt;
    std::string enc_kek;
    std::string nonce;
    std::string updated_at;

    VaultCrypto() = default;

    VaultCrypto(std::string salt_val, std::string enc, std::string nonce_val, std::string updated)
        : salt(std::move(salt_val)), enc_kek(std::move(enc)), nonce(std::move(nonce_val)), updated_at(std::move(updated)) {}

    [[nodiscard]] nlohmann::json to_json() const;
    static VaultCrypto from_json(const nlohmann::json& j);
};

#endif // VAULTCRYPTOINFO_H
