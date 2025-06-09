//
// Created by eddie phelan on 08/06/2025.
//

#ifndef VAULT_H
#define VAULT_H
#pragma once

#include <string>
#include <unordered_map>

class Vault {
public:
    static bool load_or_initialize();

    static bool unlock(const std::string& password);

    static bool change_password(const std::string& new_password);

    static void set_entry(const std::string& key, const std::string& value);

    static std::string get_entry(const std::string& key);

    static void delete_entry(const std::string& key);

    static bool save();

private:
    static inline std::string master_password;
    static inline std::string kek;

    static inline std::unordered_map<std::string, std::string> encrypted_entries;

    static bool load_from_file();
    static bool save_to_file();
};


#endif
