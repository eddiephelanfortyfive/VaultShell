//
// Created by eddie phelan on 09/06/2025.
//
#include "models/VaultCrypto.h"
nlohmann::json VaultCrypto::to_json() const {
    return {
            {"salt", salt},
            {"enc_kek", enc_kek},
            {"nonce", nonce},
            {"updated_at", updated_at}
    };
}

VaultCrypto VaultCrypto::from_json(const nlohmann::json& j) {
    return VaultCrypto{
        j.at("salt").get<std::string>(),
        j.at("enc_kek").get<std::string>(),
        j.at("nonce").get<std::string>(),
        j.at("updated_at").get<std::string>()
    };
}
