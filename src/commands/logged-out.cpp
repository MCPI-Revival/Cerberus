#include <libreborn/util/string.h>
#include <mods/misc/misc.h>

#include "commands.h"
#include "../mod.h"

// Check Username
bool is_username_valid(std::string username) {
    // Block Empty
    if (username.empty()) {
        return false;
    }
    // Convert To CP-437
    username = to_cp437(username);
    // Sanitize
    std::string sanitized_username = username;
    misc_sanitize_username(sanitized_username);
    // Check
    return username == sanitized_username;
}

// Load Commands
void add_logged_out_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid) {
    // Login
    commands.push_back({
        .name = "login",
        .args = {username_arg, password_arg},
        .callback = [self, &guid](const std::vector<std::string> &args) {
            // Arguments
            const std::string &username = args[0];
            const std::string &password = args[1];

            // Check If User Already Logged In
            bool already_logged_in = false;
            const Level *level = self->level;
            if (level) {
                for (const Player *other : level->players) {
                    if (misc_get_player_username_utf(other) == username) {
                        already_logged_in = true;
                        break;
                    }
                }
            }

            // Try To Log In
            std::string message;
            if (is_username_valid(username) && attempt_login(username, password)) {
                // Success
                if (!already_logged_in) {
                    message = "Welcome, " + username + '!';
                    login(self, guid, username);
                } else {
                    // Already logged In
                    message = "User Already Logged In";
                }
            } else {
                // Failure
                message = std::string(invalid_username) + "/Password";
            }

            // Return
            return std::vector{message};
        }
    });
}