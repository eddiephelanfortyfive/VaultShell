//
// Created by eddie phelan on 08/06/2025.
//
#include <iostream>
#include "Shell.h"
#include "src/vault/Vault.h"


int main() {
    std::cout << "🔐 Welcome to my VaultShell!" << std::endl;

    try {
        if (!Vault::load_or_initialize()) {
            std::cerr << "🚨Failed to Initialise vault🚨" << std::endl;
            return 1;
        }

        Shell shell;
        shell.run();

    } catch (const std::exception& ex) {
        std::cerr << "⚠️ Error: " << ex.what() << "⚠️"<< std::endl;
        return 1;
    }

    return 0;
}
