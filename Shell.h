// Created by eddie phelan on 08/06/2025.
//

#ifndef SHELL_HPP
#define SHELL_HPP

#include <string>

class Shell {
public:
    // Starts the shell, prompts for password, then runs command loop if unlocked
    static void run();

private:
    static void process_command(const std::string& command);

    static void cmd_ls();
    static void cmd_add(const std::string& key);
    static void cmd_delete(const std::string& key);
    static void cmd_help();
    static void cmd_change_password();
    static void cmd_change_entry_password(const std::string& key);
    static void cmd_exit();

    // Flag to track if shell should keep running
    static bool running;
};

#endif // SHELL_HPP

