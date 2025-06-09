cmake_minimum_required(VERSION 3.31)
project(VaultShell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(HEADERS
        Shell.h
        vault/Vault.h
        utils/Base64.h
        models/KEKInfo.h
        models/Vault_Entry.h
)

set(SOURCES
        main.cpp
        Shell.cpp
        vault/Vault.cpp
        crypto/Crypto.cpp
)


add_executable(VaultShell
        ${HEADERS}
        ${SOURCES}
)
