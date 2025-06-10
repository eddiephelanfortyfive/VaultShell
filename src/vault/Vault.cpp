// Vault.cpp
#include "vault/Vault.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "crypto/Crypto.h"
#include "utils/Base64.h"
#include "external/nlohmann/json.hpp"

const std::string VAULT_FILE = "vault.json";

std::vector<std::string> Vault::list_keys() {
    std::vector<std::string> keys;
    keys.reserve(encrypted_entries.size());
for (const auto& pair : encrypted_entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Vault::load_or_initialize() {
    if (std::filesystem::exists(VAULT_FILE)) {
        return load_from_file();
    } else {
        std::string salt = Crypto::generate_salt();

        std::cout << "Set a master password: ";
        std::string password;
        std::getline(std::cin, password);
        if (password.empty()) {
            std::cerr << "Password cannot be empty.\n";
            return false;
        }
        master_password = password;

        auto master_key_bytes = Crypto::derive_master_key(password, salt);
        auto master_key_base64 = Base64::encode(master_key_bytes);

        std::string enc_kek_base64, nonce_base64;
        if (!Crypto::encrypt_kek_with_master(master_key_base64, enc_kek_base64, nonce_base64)) {
            std::cerr << "Failed to encrypt KEK with master key.\n";
            return false;
        }

        auto timestamp = current_timestamp();

        nlohmann::json j;
        j["crypto_info"] = {
            {"salt", salt},
            {"enc_kek", enc_kek_base64},
            {"nonce", nonce_base64},
            {"updated_at", timestamp}
        };
        j["entries"] = nlohmann::json::object();

        std::ofstream file(VAULT_FILE);
        if (!file) {
            std::cerr << "Failed to open vault file for writing.\n";
            return false;
        }
        file << j.dump(4);
        file.close();

        salt_value = salt;
        kek = enc_kek_base64;
        nonce = nonce_base64;
        encrypted_entries.clear();

        return true;
    }
}

bool Vault::unlock(const std::string& password) {
    if (!std::filesystem::exists(VAULT_FILE)) {
        std::cerr << "Vault file not found.\n";
        return false;
    }

    std::ifstream file(VAULT_FILE);
    if (!file) {
        std::cerr << "Vault file unreadable.\n";
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse vault file: " << e.what() << "\n";
        return false;
    }

    if (!j.contains("crypto_info") || !j.contains("entries")) {
        std::cerr << "Invalid vault file format.\n";
        return false;
    }

    const auto& crypto_info = j["crypto_info"];
    salt_value = crypto_info.at("salt").get<std::string>();
    kek = crypto_info.at("enc_kek").get<std::string>();
    nonce = crypto_info.at("nonce").get<std::string>();

    auto master_key_bytes = Crypto::derive_master_key(password, salt_value);
    auto master_key_base64 = Base64::encode(master_key_bytes);

    if (!Crypto::decrypt_kek_with_master(master_key_base64, kek, nonce)) {
        std::cerr << "Failed to decrypt KEK. Wrong password?\n";
        return false;
    }

    master_password = password;

    encrypted_entries.clear();
    for (auto it = j["entries"].begin(); it != j["entries"].end(); ++it) {
        encrypted_entries[it.key()] = it.value().get<std::string>();
    }

    return true;
}

bool Vault::change_password(const std::string& new_password) {
    if (master_password.empty()) {
        std::cerr << "Vault is locked. Unlock it first.\n";
        return false;
    }

    auto old_master_key_bytes = Crypto::derive_master_key(master_password, salt_value);
    auto old_master_key_base64 = Base64::encode(old_master_key_bytes);

    if (!Crypto::decrypt_kek_with_master(old_master_key_base64, kek, nonce)) {
        std::cerr << "Failed to decrypt KEK with old master key.\n";
        return false;
    }
    std::string decrypted_kek = kek; 

    std::string new_salt = Crypto::generate_salt();
    auto new_master_key_bytes = Crypto::derive_master_key(new_password, new_salt);
    auto new_master_key_base64 = Base64::encode(new_master_key_bytes);

    std::string new_enc_kek, new_nonce;
    if (!Crypto::encrypt_kek_with_master(new_master_key_base64, new_enc_kek, new_nonce)) {
        std::cerr << "Failed to encrypt KEK with new master key.\n";
        return false;
    }

    salt_value = new_salt;
    kek = new_enc_kek;
    nonce = new_nonce;
    master_password = new_password;

    return save_to_file();
}

void Vault::set_entry(const std::string& key, const std::string& value) {
    if (master_password.empty()) {
        std::cerr << "Vault is locked. Unlock it first.\n";
        return;
    }

    std::string encrypted_value, nonce_val;
    if (!Crypto::encrypt_with_kek(Base64::decode(kek), value, key, encrypted_value,  nonce_val)) {
        std::cerr << "Failed to encrypt entry value.\n";
        return;
    }

    std::string combined = nonce_val + ":" + encrypted_value;
    encrypted_entries[key] = combined;
}

std::string Vault::get_entry(const std::string& key) {
    if (master_password.empty()) {
        std::cerr << "Vault is locked. Unlock it first.\n";
        return {};
    }

    auto it = encrypted_entries.find(key);
    if (it == encrypted_entries.end()) {
        std::cerr << "Entry not found.\n";
        return {};
    }

    auto pos = it->second.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Corrupted entry format.\n";
        return {};
    }
    std::string nonce_val = it->second.substr(0, pos);
    std::string encrypted_value = it->second.substr(pos + 1);

    std::string decrypted_value;

    if (!Crypto::decrypt_with_kek(Base64::decode(kek), encrypted_value, key,  nonce_val, decrypted_value)) {
        std::cerr << "Failed to decrypt entry.\n";
        return {};
    }

    return decrypted_value;
}

void Vault::delete_entry(const std::string& key) {
    encrypted_entries.erase(key);
}

bool Vault::save() {
    if (master_password.empty()) {
        std::cerr << "Vault is locked. Unlock it first.\n";
        return false;
    }
    return save_to_file();
}

bool Vault::save_to_file() {
    nlohmann::json j;

    j["crypto_info"] = {
        {"salt", salt_value},
        {"enc_kek", kek},
        {"nonce", nonce},
        {"updated_at", current_timestamp()}
    };

    nlohmann::json entries_json = nlohmann::json::object();
    for (const auto& pair : encrypted_entries) {
        entries_json[pair.first] = pair.second;
    }
    j["entries"] = entries_json;

    std::ofstream file(VAULT_FILE);
    if (!file) {
        std::cerr << "Failed to open vault file for writing.\n";
        return false;
    }
    file << j.dump(4);
    file.close();
    return true;
}

std::string Vault::current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%FT%TZ");  // ISO8601 UTC time
    return ss.str();
}

bool Vault::load_from_file() {
    const std::string filename = "vault.json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Vault file not found: " << filename << std::endl;
        return false;
    }

    nlohmann::json root;
    try {
        file >> root;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse vault JSON: " << e.what() << std::endl;
        return false;
    }
    file.close();

    if (!root.contains("crypto_info") || !root.contains("entries")) {
        std::cerr << "Vault file missing required fields." << std::endl;
        return false;
    }

    const auto& crypto_info = root["crypto_info"];

    if (!crypto_info.contains("enc_kek") || !crypto_info.contains("salt") || !crypto_info.contains("nonce")) {
        std::cerr << "Vault file crypto_info missing required fields." << std::endl;
        return false;
    }

    try {
        kek = crypto_info.at("enc_kek").get<std::string>();
        salt_value = crypto_info.at("salt").get<std::string>();
        nonce = crypto_info.at("nonce").get<std::string>();

        encrypted_entries.clear();
        for (auto& [key, value] : root["entries"].items()) {
            encrypted_entries[key] = value.get<std::string>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading vault data: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Vault::change_entry_password(const std::string& key, const std::string& new_password) {
    if (master_password.empty()) {
        std::cerr << "Vault is locked. Unlock it first.\n";
        return false;
    }

    if (encrypted_entries.find(key) == encrypted_entries.end()) {
        std::cerr << "Entry '" << key << "' not found.\n";
        return false;
    }

    std::string encrypted_value, nonce_val;
    if (!Crypto::encrypt_with_kek(Base64::decode(kek), new_password, key, encrypted_value, nonce_val)) {
        std::cerr << "Failed to encrypt new password for entry.\n";
        return false;
    }

    std::string combined = nonce_val + ":" + encrypted_value;
    encrypted_entries[key] = combined;

    return save_to_file();
}

bool Vault::has_entry(const std::string& key) {
    return encrypted_entries.find(key) != encrypted_entries.end();
}



