cmake_minimum_required(VERSION 3.31)
project(VaultShell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

find_package(OpenSSL REQUIRED)

find_package(Argon2 QUIET)

if (Argon2_FOUND)
    message(STATUS "Found system-wide Argon2")
    include_directories(${ARGON2_INCLUDE_DIR})
    set(ARGON2_LIB_ARG ${ARGON2_LIB})
else()
    message(WARNING "System-wide Argon2 NOT found, using local Argon2 path")

    set(ARGON2_INCLUDE_DIR "/absolute/path/to/your/local/argon2/include")
    set(ARGON2_LIB "/absolute/path/to/your/local/argon2/libargon2.a")

    include_directories(${ARGON2_INCLUDE_DIR})
    set(ARGON2_LIB_ARG ${ARGON2_LIB})
endif()

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

target_link_libraries(VaultShell
        OpenSSL::SSL
        OpenSSL::Crypto
        ${ARGON2_LIB_ARG}
)
