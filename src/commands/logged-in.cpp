#include <mods/misc/misc.h>
#include <mods/server/server.h>

#include "commands.h"
#include "../config/config.h"

// Load Commands
void add_logged_in_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid) {
    // Delete Account
    commands.push_back({
        .name = "ban",
        .requires_admin = true,
        .args = {username_arg},
        .callback = [self](const std::vector<std::string> &args) {
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
    });

    // Create Account
    commands.push_back({
        .name = "register",
        .requires_admin = true,
        .args = {username_arg, password_arg},
        .callback = [](const std::vector<std::string> &args) {
            // Arguments
            const std::string &username = args[0];
            const std::string &password = args[1];

            // Run
            std::string message;
            if (!is_username_valid(username)) {
                message = invalid_username;
            } else if (create_account(username, password)) {
                message = "Created: " + username;
            } else {
                message = "Unable To Create Account";
            }
            return std::vector{message};
        }
    });

    // Report Player
    commands.push_back({
        .name = "report",
        .args = {username_arg, "reason"},
        .callback = [self, &guid](const std::vector<std::string> &args) {
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
    });

    // Change Password
    commands.push_back({
        .name = "password",
        .args = {"old", "new"},
        .callback = [self, &guid](const std::vector<std::string> &args) {
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
    });
}