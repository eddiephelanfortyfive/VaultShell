//
// Created by eddie phelan on 08/06/2025.
//
#include <iostream>
#include "Shell.h"
#include "vault/Vault.h"
#include "utils/colours.h"

int main() {
    try {
        if (!Vault::load_or_initialize()) {
            std::cerr << COLOR_ERROR << "🚨Failed to Initialise vault🚨" << COLOR_ERROR <<std::endl;
            return 1;
        }

        Shell::run();

    } catch (const std::exception& ex) {
        std::cerr << COLOR_ERROR << "⚠️ Error: " << ex.what() << "⚠️"<< COLOR_ERROR << std::endl;
        return 1;
    }

    return 0;
}
