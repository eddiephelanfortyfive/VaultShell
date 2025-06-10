cmake_minimum_required(VERSION 3.31)
project(VaultShell)

set(CMAKE_CXX_STANDARD 20)

# Include local headers
include_directories(include)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Argon2 library and include paths (adjust if needed)
set(ARGON2_INCLUDE_DIR "/opt/homebrew/Cellar/argon2/20190702_1/include" CACHE PATH "Argon2 include dir")
set(ARGON2_LIB "/opt/homebrew/Cellar/argon2/20190702_1/lib/libargon2.dylib" CACHE FILEPATH "Argon2 library path")

# Include Argon2 headers
include_directories(${ARGON2_INCLUDE_DIR})

set(HEADERS
        Shell.h
        include/vault/Vault.h
        include/utils/Base64.h
        include/models/VaultCrypto.h
        include/models/VaultEntry.h
        include/crypto/Crypto.h
        include/utils/colours.h
        include/utils/input.h
)

set(SOURCES
        main.cpp
        Shell.cpp
        src/vault/Vault.cpp
        src/crypto/Crypto.cpp
        src/utils/Base64.cpp
        src/models/VaultEntry.cpp
        src/models/VaultCrypto.cpp
        src/utils/input.cpp
)

add_executable(VaultShell
        ${HEADERS}
        ${SOURCES}
)

# Link OpenSSL and Argon2 libraries
target_link_libraries(VaultShell
        OpenSSL::SSL
        OpenSSL::Crypto
        ${ARGON2_LIB}
)
