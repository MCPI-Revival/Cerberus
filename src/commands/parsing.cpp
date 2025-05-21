#include <algorithm>
#include <libreborn/log.h>
#include <libreborn/util/string.h>

#include "commands.h"
#include "../mod.h"

// Parsing Arguments
static bool parse_two_args(const std::string &command, std::string &a, std::string &b) {
    const std::string::size_type divider = command.find(' ');
    if (divider == std::string::npos) {
        return false;
    }
    a = command.substr(0, divider);
    b = command.substr(divider + 1);
    return !a.empty() && !b.empty();
}

// Execute Command
static std::vector<std::string> run_command(const std::string &input, const Command &command) {
    // Parse
    std::vector<std::string> args;
    const int arg_count = int(command.args.size());
    static constexpr const char *invalid_arguments = "Invalid Arguments";
    if (arg_count == 2) {
        // Two Arguments
        std::string a;
        std::string b;
        if (!parse_two_args(input, a, b)) {
            return {invalid_arguments};
        }
        args = {a, b};
    } else if (arg_count == 1) {
        // Only One Argument
        if (input.empty()) {
            return {invalid_arguments};
        }
        args = {input};
    } else if (arg_count == 0) {
        // No Arguments
    } else {
        // Not Supported
        IMPOSSIBLE();
    }
    // Run
    return command.callback(args);
}

// Determine Which Command To Run, And Run It
static bool run(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &input, std::vector<Command> &commands) {
    // Sort Commands
    std::ranges::sort(commands, [](const Command &a, const Command &b) {
        return a.name < b.name;
    });

    // Add Slash To Commands
    for (Command &command : commands) {
        command.name.insert(0, 1, '/');
    }

    // Remove Prohibited Commands
    bool admin = false;
    const Player *player = self->getPlayer(guid);
    if (player) {
        admin = is_admin(player);
    }
    if (!admin) {
        std::erase_if(commands, [](const Command &command) {
            return command.requires_admin;
        });
    }

    // Parse
    for (const Command &command : commands) {
        // Check Command
        const std::string prefix = command.name + ' ';
        if (input.starts_with(prefix) || input == command.name) {
            // Extract Arguments
            std::string args_str;
            if (input.size() >= prefix.size()) {
                args_str = input.substr(prefix.size());
            }
            // Run
            const std::vector<std::string> output = run_command(args_str, command);
            for (const std::string &line : output) {
                tell(self, guid, line);
            }
            return true;
        }
    }

    // No Command Was Run
    return false;
}

// Handle Chat Message
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const bool logged_in, std::string command) {
    // Convert To Unicode
    command = from_cp437(command);

    // Common Commands
    std::vector<Command> commands;
    add_common_commands(commands, self);

    // Enable/Disable Commands Based On Logged-In State
    if (!logged_in) {
        // Logged-Out Commands
        add_logged_out_commands(commands, self, guid);

        // Run
        run(self, guid, command, commands);
        // Always Swallow Message
        return true;
    } else {
        // Logged-In Commands
        add_logged_in_commands(commands, self, guid);

        // Run
        return run(self, guid, command, commands);
    }
}