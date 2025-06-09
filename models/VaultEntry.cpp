//
// Created by eddie phelan on 09/06/2025.
//
#include "VaultEntry.h"

VaultEntry::VaultEntry(const std::string &n, const std::string &val, const std::string &nonce_val,
                       const std::string &created, const std::string &updated)
    : name(n), value(val), nonce(nonce_val), created_at(created), updated_at(updated) {}

nlohmann::json VaultEntry::to_json() const {
    return {
            {"name", name},
            {"value", value},
            {"nonce", nonce},
            {"created_at", created_at},
            {"updated_at", updated_at}
    };
}

VaultEntry VaultEntry::from_json(const nlohmann::json& j) {
    return VaultEntry{
        j.at("name").get<std::string>(),
        j.at("value").get<std::string>(),
        j.at("nonce").get<std::string>(),
        j.at("created_at").get<std::string>(),
        j.at("updated_at").get<std::string>()
    };
}

