//
// Created by eddie phelan on 08/06/2025.
//

#ifndef VAULT_ENTRY_H
#define VAULT_ENTRY_H

#include <string>

class Vault_Entry {
public:
    std::string name;       // Entry name or key (e.g., "gmail")
    std::string enc_value;  // Encrypted password or secret (Base64 encoded)
    std::string nonce;      // Nonce or IV used for encryption (Base64 encoded)
    std::string created_at; // Optional: creation timestamp
    std::string updated_at; // Optional: last updated timestamp

    Vault_Entry() = default;
    Vault_Entry(const std::string& n, const std::string& val, const std::string& nonce_val,
                const std::string& created = "", const std::string& updated = "");

    // Serialization / Deserialization helpers (optional)
};

#endif // VAULT_ENTRY_H
