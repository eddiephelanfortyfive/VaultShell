// Created by eddie phelan on 08/06/2025.

#include "Shell.h"
#include "vault/Vault.h"
#include "utils/colours.h"
#include "utils/input.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

bool Shell::running = true;

void Shell::run() {
    print_info("Welcome to the Vault Shell.");

    while (true) {
        std::string password = get_hidden_input("Enter master password: ");

        if (password.empty()) {
            print_error("Password cannot be empty. Try again.");
            continue;
        }

        if (Vault::unlock(password)) {
            print_success("Vault unlocked successfully.");
            break;
        } else {
            print_error("Incorrect password. Try again or press Ctrl+C to exit.");
        }
    }

    print_info("Type 'help' for a list of commands.");

    while (running) {
        std::cout << COLOR_PROMPT << "vaultShell> " << COLOR_RESET;
        std::string line;
        if (!std::getline(std::cin, line)) {
            print_success("\nExiting vaultShell.");
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
    } else {
        print_error("Unknown command: " + cmd);
        print_info("Type 'help' for a list of commands.");
    }
}

void Shell::cmd_ls() {
    auto keys = Vault::list_keys();
    if (keys.empty()) {
        print_error("Vault is empty.");
        return;
    }

    // Define column widths
    const int name_width = 22;
    const int user_width = 30;
    const int pass_width = 30;

    std::string line = std::string(82, '=');
    print_plain(line);
    std::ostringstream header;
    header << std::left
           << std::setw(name_width) << "Name" << " | "
           << std::setw(user_width) << "Username" << " | "
           << std::setw(pass_width) << "Password";
    print_plain(header.str());
    print_plain(line);

    for (const auto& key : keys) {
        std::string value = Vault::get_entry(key);
        std::string username, password;

        size_t delim_pos = value.find(':');
        if (delim_pos != std::string::npos) {
            username = value.substr(0, delim_pos);
            password = value.substr(delim_pos + 1);
        } else {
            username = "(unknown)";
            password = "(unknown)";
        }

        std::ostringstream oss;
        oss << std::left
            << std::setw(name_width) << key << " | "
            << std::setw(user_width) << username << " | "
            << std::setw(pass_width) << password;

        print_plain(oss.str());
    }

    print_plain(line);
}



void Shell::cmd_add(const std::string& name) {
    if (name.empty()) {
        print_info("Usage: add <site/website>");
        return;
    }

    std::string username, password;

    std::cout << "Enter username for '" << name << "': ";
    std::getline(std::cin, username);

    if (username.empty()) {
        print_error("Username cannot be empty.");
        return;
    }

    std::cout << "Enter password for '" << name << "': ";
    std::getline(std::cin, password);

    if (password.empty()) {
        print_error("Password cannot be empty.");
        return;
    }

    // Store as "username:password" (or you can JSON structure this if you want richer storage)
    std::string value = username + ":" + password;

    Vault::set_entry(name, value);
    if (!Vault::save()) {
        print_error("Failed to save vault.");
        return;
    }
    print_success("Entry '" + name + "' added.");
}


void Shell::cmd_delete(const std::string& key) {
    if (key.empty()) {
        print_info("Usage: delete <site/website>");
        return;
    }

    if (!Vault::has_entry(key)) {
        print_error("No such entry found: '" + key + "'.");
        return;
    }

    Vault::delete_entry(key);
    if (!Vault::save()) {
        print_error("Failed to save vault.");
        return;
    }

    print_success("Entry '" + key + "' deleted.");
}


void Shell::cmd_help() {
    print_plain("Available commands:");
    print_plain("  ls               - List all entries");
    print_plain("  add <name>       - Add a new entry");
    print_plain("  delete <name>    - Delete an entry");
    print_plain("  help             - Show this help message");
    print_plain("  change           - Changing master password");
    print_plain("  change <name>    - Changing entry password");
    print_plain("  exit             - Exit the shell");
}

void Shell::cmd_exit() {
    print_success("Exiting shell.");
    running = false;
}

void Shell::cmd_change_password() {
    std::string new_password, confirm_password;

    std::cout << "Enter new master password: ";
    std::getline(std::cin, new_password);

    if (new_password.empty()) {
        print_error("Password cannot be empty.");
        return;
    }

    std::cout << "Confirm new master password: ";
    std::getline(std::cin, confirm_password);

    if (new_password != confirm_password) {
        print_error("Passwords do not match.");
        return;
    }

    if (Vault::change_password(new_password)) {
        print_success("Master password changed successfully.");
    } else {
        print_error("Failed to change master password.");
    }
}

void Shell::cmd_change_entry_password(const std::string& key) {
    if (key.empty()) {
        print_info("Usage: change <name>");
        return;
    }

    if (!Vault::has_entry(key)) {
        print_error("No such entry found.");
        return;
    }

    std::string new_password;
    std::cout << "Enter new password for '" << key << "': ";
    std::getline(std::cin, new_password);

    if (new_password.empty()) {
        print_error("Password cannot be empty.");
        return;
    }

    Vault::set_entry(key, new_password);
    if (!Vault::save()) {
        print_error("Failed to save vault.");
        return;
    }

    print_success("Password for '" + key + "' updated successfully.");
}

// Helper functions
void Shell::print_error(const std::string& message) {
    std::cout << COLOR_ERROR << message << COLOR_RESET << std::endl;
}

void Shell::print_info(const std::string& message) {
    std::cout << COLOR_INFO << message << COLOR_RESET << std::endl;
}

void Shell::print_success(const std::string& message) {
    std::cout << COLOR_SUCCESS << message << COLOR_RESET << std::endl;
}

void Shell::print_plain(const std::string& message) {
    std::cout << COLOR_PLAIN << message << COLOR_RESET << std::endl;
}


