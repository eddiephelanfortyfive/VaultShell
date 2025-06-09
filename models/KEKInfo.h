//
// Created by eddie phelan on 08/06/2025.
//

#ifndef KEKINFO_H
#define KEKINFO_H

#include <string>

class KEKInfo {
public:
    std::string enc_kek;    // Encrypted Key Encryption Key (Base64 encoded)
    std::string nonce;      // Nonce or IV used for KEK encryption (Base64 encoded)
    std::string updated_at; // Optional: timestamp of last update (ISO8601 string)

    KEKInfo() = default;
    KEKInfo(const std::string& enc, const std::string& nonce_val, const std::string& updated = "");

    // Serialization / Deserialization helpers (optional)
    // e.g., to/from JSON string or nlohmann::json object
};

#endif // KEKINFO_H
