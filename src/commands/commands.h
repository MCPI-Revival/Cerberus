#pragma once

#include <string>
#include <vector>
#include <functional>

#include <symbols/minecraft.h>

// Command Description
struct Command {
    std::string name;
    bool requires_admin = false;
    std::vector<std::string> args = {};
    std::function<std::vector<std::string>(const std::vector<std::string> &)> callback;
};

// Common Arguments
static constexpr const char *username_arg = "username";
static constexpr const char *password_arg = "password";
// Common Errors
static constexpr const char *invalid_player = "Invalid Player";
static constexpr const char *invalid_username = "Invalid Username";

// Check Username
bool is_username_valid(std::string username);

// Load Commands
void add_common_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self);
void add_logged_out_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid);
void add_logged_in_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid);