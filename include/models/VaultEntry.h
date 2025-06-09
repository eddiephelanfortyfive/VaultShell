//
// Created by eddie phelan on 08/06/2025.
//

#ifndef VAULT_ENTRY_H
#define VAULT_ENTRY_H

#include <string>
#include <external/nlohmann/json.hpp>

class VaultEntry {
public:
    std::string name;
    std::string username;
    std::string value;
    std::string nonce;
    std::string created_at;
    std::string updated_at;

    VaultEntry() = default;
    VaultEntry(std::string  n, std::string  val, std::string usern, std::string  nonce_val,
               std::string  created, std::string  updated);

    [[nodiscard]] nlohmann::json to_json() const;
    static VaultEntry from_json(const nlohmann::json& j);
};

#endif // VAULT_ENTRY_H
