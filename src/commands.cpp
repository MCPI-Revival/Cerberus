#include <libreborn/util/string.h>

#include <mods/misc/misc.h>
#include <mods/server/server.h>

#include "mod.h"

// Commands
#define def(name) static constexpr const char *name##_command = "/" #name " "
def(login);
def(ban);
def(register);
def(report);
#undef def

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

// Handle
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const bool logged_in, std::string command) {
    static constexpr const char *invalid_arguments = "Invalid Arguments";
    command = from_cp437(command);
    if (!logged_in) {
        // Logged-Out
        if (command.starts_with(login_command)) {
            // Login
            command = command.substr(strlen(login_command));
            std::string username;
            std::string password;
            const bool valid_args = parse_two_args(command, username, password);

            // Attempt
            std::string message;
            if (!valid_args) {
                message = invalid_arguments;
            } else {
                // Sanitize Username
                username = to_cp437(username);
                misc_sanitize_username(username);
                const std::string username_utf = from_cp437(username);
                // Try To Log In
                if (attempt_login(username_utf, password)) {
                    // Success
                    message = "Welcome, " + username_utf + '!';
                    login(self, guid, username);
                } else {
                    // Failure
                    message = "Invalid Username/Password";
                }
            }
            // Return
            tell(self, guid, message);
        }
        return true;
    } else {
        // Logged-In
        static constexpr const char *invalid_player = "Invalid Player";
        const Player *player = self->getPlayer(guid);
        const Level *level = self->level;
        if (player && level) {
            const bool admin = is_admin(player);
            std::string message;
            // Check Possible Commands
            if (admin && command.starts_with(ban_command)) {
                // Ban
                const std::string username = command.substr(strlen(ban_command));

                // Remove Account
                bool valid = delete_account(username);
                // Kick Players
                for (Player *other : self->level->players) {
                    if (misc_get_player_username_utf(other) == username) {
                        server_kick((ServerPlayer *) other);
                        valid = true;
                    }
                }
                // Return
                message = valid ? "Banned" : invalid_player;
                message += ": " + username;
            } else if (admin && command.starts_with(register_command)) {
                // Create Account
                command = command.substr(strlen(register_command));
                std::string username;
                std::string password;
                const bool valid_args = parse_two_args(command, username, password);

                // Run
                if (!valid_args) {
                    message = invalid_arguments;
                } else if (create_account(username, password)) {
                    message = "Created: " + username;
                } else {
                    message = "Account Already Exists";
                }
            } else if (command.starts_with(report_command)) {
                // Report Player
                command = command.substr(strlen(report_command));
                std::string target;
                std::string reason;
                const bool valid_args = parse_two_args(command, target, reason);

                // Run
                if (!valid_args) {
                    message = invalid_arguments;
                } else {
                    const bool valid = has_account(target);
                    if (valid) {
                        message = "Reported: " + target;
                        send_to_discord("**" + misc_get_player_username_utf(player) + " has reported " + target + " for:** " + reason, true);
                    } else {
                        message = invalid_player;
                    }
                }
            }
            // Return
            if (!message.empty()) {
                tell(self, guid, message);
                return true;
            }
        }
        return false;
    }
}