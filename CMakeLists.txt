cmake_minimum_required(VERSION 3.16.0)

# Build For ARM
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "arm")

# Start Project
project(cerberus)

# Include SDK
include("$ENV{HOME}/.minecraft-pi/sdk/lib/minecraft-pi-reborn/sdk/sdk.cmake")

# Warning
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpointer-arith
    -Wshadow
    -Wnull-dereference
)

# Password Hashing
add_library(bcrypt STATIC
    bcrypt/base64.c
    bcrypt/bcrypt.c
    bcrypt/keccak.c
    bcrypt/sha512.c
    bcrypt/crypt_blowfish/crypt_blowfish.c
    bcrypt/crypt_blowfish/crypt_gensalt.c
    bcrypt/crypt_blowfish/wrapper.c
)
target_include_directories(bcrypt PUBLIC bcrypt)
target_compile_options(bcrypt PRIVATE
    -Wno-array-parameter
    -Wno-type-limits
)

# Build
set(CMAKE_CXX_VISIBILITY_PRESET internal)
add_library(cerberus SHARED
    src/config/file.cpp
    src/config/accounts.cpp
    src/config/admins.cpp
    src/config/welcome.cpp
    src/config/webhook.cpp
    src/commands/parsing.cpp
    src/commands/common.cpp
    src/commands/logged-in.cpp
    src/commands/logged-out.cpp
    src/init.cpp
    src/login.cpp
    src/packets.cpp
    src/hash.cpp
)
target_link_libraries(cerberus mods symbols reborn-patch bcrypt)
