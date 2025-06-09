//
// Created by eddie phelan on 08/06/2025.
//
#include <iostream>
// #include "shell.hpp"
// #include "vault/vault.hpp"

int main() {
    std::cout << "🔐 Welcome to SecureVaultShell!" << std::endl;

    try {
        // Initialize or unlock the vault
        if (!Vault::load_or_initialize()) {
            std::cerr << "Failed to Initialise vault" << std::endl;
            return 1;
        }

        // Start shell loop
        Shell shell;
        shell.run();
    } catch (const std::exception& ex) {
        std::cerr << "⚠️ Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
