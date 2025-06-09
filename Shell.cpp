//
// Created by eddie phelan on 08/06/2025.
//

#include "Shell.h"
#include "vault/Vault.h"

#include <iostream>
#include <sstream>
#include <string>

bool Shell::running = true;

void Shell::run() {
    std::cout << "Welcome to the Vault Shell.\n";

    while (true) {
        std::cout << "Enter master password: ";
        std::string password;
        std::getline(std::cin, password);

        if (password.empty()) {
            std::cout << "Password cannot be empty. Try again.\n";
            continue;
        }

        if (Vault::unlock(password)) {
            std::cout << "Vault unlocked successfully.\n";
            break;
        } else {
            std::cout << "Incorrect password. Try again or press Ctrl+C to exit.\n";
        }
    }

    std::cout << "Type 'help' for a list of commands.\n";

    while (running) {
        std::cout << "\033[36m" << "vaultShell> " << "\033[0m";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nExiting vaultShell.\n";
            break;
        }

        process_command(line);
    }
}

void Shell::process_command(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, arg;
    iss >> cmd >> arg;

    if (cmd == "ls") {
        cmd_ls();
    } else if (cmd == "add") {
        cmd_add(arg);
    } else if (cmd == "delete") {
        cmd_delete(arg);
    } else if (cmd == "help") {
        cmd_help();
    } else if (cmd == "exit") {
        cmd_exit();
    } else if (cmd == "change" && arg.empty()) {
        cmd_change_password();
    } else if (cmd == "change" && !arg.empty()) {
        cmd_change_entry_password(arg);
    } else if (cmd.empty()) {
        // Ignore empty command
    } else {
        std::cout << "Unknown command: " << cmd << "\n";
        std::cout << "Type 'help' for a list of commands.\n";
    }
}



void Shell::cmd_ls() {
    auto keys = Vault::list_keys();
    if (keys.empty()) {
        std::cout << "Vault is empty.\n";
        return;
    }

    for (const auto& key : keys) {
        std::string password = Vault::get_entry(key);
        std::cout << key << ": " << password << "\n";
    }
}


void Shell::cmd_add(const std::string& key) {
    if (key.empty()) {
        std::cout << "Usage: add <site/website>\n";
        return;
    }

    std::string value;
    std::cout << "Enter password for '" << key << "': ";
    std::getline(std::cin, value);

    if (value.empty()) {
        std::cout << "Password cannot be empty.\n";
        return;
    }

    Vault::set_entry(key, value);
    if (!Vault::save()) {
        std::cout << "Failed to save vault.\n";
        return;
    }
    std::cout << "Entry '" << key << "' added.\n";
}


void Shell::cmd_delete(const std::string& key) {
    if (key.empty()) {
        std::cout << "Usage: delete <site/website>\n";
        return;
    }

    Vault::delete_entry(key);
    if (!Vault::save()) {
        std::cout << "Failed to save vault.\n";
        return;
    }
    std::cout << "Entry '" << key << "' deleted.\n";
}


void Shell::cmd_help() {
    std::cout << "Available commands:\n";
    std::cout << "  ls      - List all entries\n";
    std::cout << "  add     - Add a new entry\n";
    std::cout << "  delete  - Delete an entry\n";
    std::cout << "  help    - Show this help message\n";
    std::cout << "  cmaster    - Changing master password\n";
    std::cout << "  centry    - Changing entry password\n";
    std::cout << "  exit    - Exit the shell\n";
}

void Shell::cmd_exit() {
    std::cout << "Exiting shell.\n";
    running = false;
}

// In Shell.cpp, add the new command handler:

void Shell::cmd_change_password() {
    std::string new_password, confirm_password;

    std::cout << "Enter new master password: ";
    std::getline(std::cin, new_password);

    if (new_password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return;
    }

    std::cout << "Confirm new master password: ";
    std::getline(std::cin, confirm_password);

    if (new_password != confirm_password) {
        std::cout << "Passwords do not match.\n";
        return;
    }

    if (Vault::change_password(new_password)) {
        std::cout << "Master password changed successfully.\n";
    } else {
        std::cout << "Failed to change master password.\n";
    }
}

void Shell::cmd_change_entry_password(const std::string& key) {
    if (key.empty()) {
        std::cout << "Usage: cepw <site/website>\n";
        return;
    }

    if (!Vault::has_entry(key)) {
        std::cout << "No such entry found.\n";
        return;
    }

    std::string new_password;
    std::cout << "Enter new password for '" << key << "': ";
    std::getline(std::cin, new_password);

    if (new_password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return;
    }

    Vault::set_entry(key, new_password);
    if (!Vault::save()) {
        std::cout << "Failed to save vault.\n";
        return;
    }

    std::cout << "Password for '" << key << "' updated successfully.\n";
}



