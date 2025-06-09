//
// Created by eddie phelan on 09/06/2025.
//
#include "models/VaultEntry.h"

#include <utility>
#include "external/nlohmann/json.hpp"
VaultEntry::VaultEntry(std::string n, std::string val, std::string nonce_val,
                       std::string created, std::string updated)
    : name(std::move(n)), value(std::move(val)), nonce(std::move(nonce_val)), created_at(std::move(created)), updated_at(std::move(updated)) {}

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

