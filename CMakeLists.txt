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
add_compile_options(-Wall -Wextra -Werror -Wpointer-arith -Wshadow -Wnull-dereference)

# Build
set(CMAKE_CXX_VISIBILITY_PRESET internal)
add_library(cerberus SHARED
    src/accounts.cpp
    src/admins.cpp
    src/commands/parsing.cpp
    src/commands/common.cpp
    src/commands/logged-in.cpp
    src/commands/logged-out.cpp
    src/init.cpp
    src/login.cpp
    src/packets.cpp
    src/webhook.cpp
    src/hash.cpp
    sha256/src/SHA256.cpp
)
target_link_libraries(cerberus mods symbols reborn-patch)
target_include_directories(cerberus PRIVATE sha256/include)
