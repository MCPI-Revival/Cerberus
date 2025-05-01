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

# Build
set(CMAKE_CXX_VISIBILITY_PRESET internal)
add_library(cerberus SHARED
    src/accounts.cpp
    src/admins.cpp
    src/commands.cpp
    src/init.cpp
    src/login.cpp
    src/packets.cpp
    src/webhook.cpp
)
target_link_libraries(cerberus mods symbols reborn-patch)
