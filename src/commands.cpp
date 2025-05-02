#include <libreborn/util/string.h>
#include <libreborn/log.h>

#include <mods/misc/misc.h>
#include <mods/server/server.h>

#include "mod.h"

// Parsing
static bool parse_two_args(const std::string &command, std::string &a, std::string &b) {
    const std::string::size_type divider = command.find(' ');
    if (divider == std::string::npos) {
        return false;
    }
    a = command.substr(0, divider);
    b = command.substr(divider + 1);
    return !a.empty() && !b.empty();
}

// Parse
struct Command {
    std::string name;
    bool requires_admin = false;
    std::vector<std::string> args = {};
    std::function<std::vector<std::string>(const std::vector<std::string> &)> callback;
};
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
    // Generate Help
    commands.push_back({
        .name = "help",
        .callback = [&commands](__attribute__((unused)) const std::vector<std::string> &args) {
            std::vector<std::string> ret = {"All Commands:"};
            for (const Command &command : commands) {
                std::string line = "- " + command.name;
                for (const std::string &arg : command.args) {
                    line += " <" + arg + '>';
                }
                ret.push_back(line);
            }
            return ret;
        }
    });

    // Add Slash To Commands
    for (Command &command : commands) {
        command.name = '/' + command.name;
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

// Handle
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const bool logged_in, std::string command) {
    // Common Arguments
    static constexpr const char *username_arg = "username";
    static constexpr const char *password_arg = "password";
    // Convert To Unicode
    command = from_cp437(command);
    // Run
    if (!logged_in) {
        // Logged-Out
        std::vector<Command> commands = {
            // Login
            {
                .name = "login",
                .args = {username_arg, password_arg},
                .callback = [&self, &guid](const std::vector<std::string> &args) {
                    std::string message;
                    // Arguments
                    std::string username_cp437 = to_cp437(args[0]);
                    misc_sanitize_username(username_cp437);
                    const std::string username_utf = from_cp437(username_cp437);
                    const std::string &password = args[1];
                    // Try To Log In
                    if (attempt_login(username_utf, password)) {
                        // Success
                        message = "Welcome, " + username_utf + '!';
                        login(self, guid, username_cp437);
                    } else {
                        // Failure
                        message = "Invalid Username/Password";
                    }
                    // Return
                    return std::vector{message};
                }
            }
        };
        run(self, guid, command, commands);
        return true;
    } else {
        // Logged-In
        static constexpr const char *invalid_player = "Invalid Player";
        std::vector<Command> commands = {
            // Delete Account
            {
                .name = "ban",
                .requires_admin = true,
                .args = {username_arg},
                .callback = [&self](const std::vector<std::string> &args) {
                    // Arguments
                    const std::string &username = args[0];
                    // Remove Account
                    bool valid = delete_account(username);
                    // Kick Players
                    const Level *level = self->level;
                    if (level) {
                        for (Player *other : level->players) {
                            if (misc_get_player_username_utf(other) == username) {
                                server_kick((ServerPlayer *) other);
                                valid = true;
                            }
                        }
                    }
                    // Return
                    std::string message = valid ? "Banned" : invalid_player;
                    message += ": " + username;
                    return std::vector{message};
                }
            },
            // Create Account
            {
                .name = "register",
                .requires_admin = true,
                .args = {username_arg, password_arg},
                .callback = [](const std::vector<std::string> &args) {
                    // Arguments
                    const std::string &username = args[0];
                    const std::string &password = args[1];
                    // Run
                    std::string message;
                    if (create_account(username, password)) {
                        message = "Created: " + username;
                    } else {
                        message = "Account Already Exists";
                    }
                    return std::vector{message};
                }
            },
            // Report Player
            {
                .name = "report",
                .args = {username_arg, "reason"},
                .callback = [&self, &guid](const std::vector<std::string> &args) {
                    // Arguments
                    const std::string &target = args[0];
                    const std::string &reason = args[1];
                    // Get Reporter
                    std::string reporter;
                    const Player *player = self->getPlayer(guid);
                    if (player) {
                        reporter = misc_get_player_username_utf(player);
                    }
                    // Run
                    std::string message;
                    const bool valid = has_account(target);
                    if (valid) {
                        message = "Reported: " + target;
                        send_to_discord("**" + reporter + " has reported " + target + " for:** " + reason, true);
                    } else {
                        message = invalid_player;
                    }
                    return std::vector{message};
                }
            },
            // Change Password
            {
                .name = "password",
                .args = {"old", "new"},
                .callback = [&self, &guid](const std::vector<std::string> &args) {
                    // Arguments
                    const std::string &old_password = args[0];
                    const std::string &new_password = args[1];
                    // Get Username
                    std::string username;
                    const Player *player = self->getPlayer(guid);
                    if (player) {
                        username = misc_get_player_username_utf(player);
                    }
                    // Run
                    std::string message = "Invalid Password";
                    if (change_password(username, old_password, new_password)) {
                        message = "Password Changed";
                    }
                    return std::vector{message};
                }
            }
        };
        return run(self, guid, command, commands);
    }
}