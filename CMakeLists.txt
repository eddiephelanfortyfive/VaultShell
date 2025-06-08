cmake_minimum_required(VERSION 3.31)
project(VaultShell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(VaultShell
        main.cpp
        shell.h
        shell.cpp
        vault/vault.h
        vault/vault.cpp
        crypto/crypto.h
        crypto/crypto.cpp
        models/vault_entry.h
        models/kek_info.h
)
