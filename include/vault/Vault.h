//
// Created by eddie phelan on 08/06/2025.
//

#ifndef VAULT_H
#define VAULT_H
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Vault {
public:
    static std::vector<std::string> list_keys();

    static bool load_or_initialize();

    static bool unlock(const std::string& password);

    static bool change_password(const std::string& new_password);

    static void set_entry(const std::string& key, const std::string& value);

    static std::string get_entry(const std::string& key);

    static void delete_entry(const std::string& key);

    static bool save();

    static bool has_entry(const std::string &key);

    static bool change_entry_password(const std::string& key, const std::string& new_password);

private:
    static inline std::string master_password;
    static inline  std::string kek;
    static inline std::string salt_value;
    static inline std::string nonce;

    static inline std::unordered_map<std::string, std::string> encrypted_entries;

    static bool load_from_file();
    static bool save_to_file();
    static std::string current_timestamp();
};


#endif
