#include <libreborn/util/string.h>
#include <libreborn/log.h>

#include <mods/api/api.h>
#include <mods/misc/misc.h>

#include "mod.h"

// Commands
#define def(name) static constexpr const char *name##_command = "/" #name " "
def(login);
#undef def

// Login Player
static void login(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &username) {
    // Mark
    mark_logged_in(guid);

    // Update Username
    bool found = false;
    for (Player *player : self->pending_players) {
        ServerPlayer *server_player = (ServerPlayer *) player;
        if (server_player->guid.equals(guid)) {
            server_player->username = username;
            found = true;
        }
    }
    if (!found) {
        IMPOSSIBLE();
    }

    // Add Player To Level
    self->onReady_ClientGeneration(guid);

    // Update Position
    Player *player = self->getPlayer(guid);
    if (player) {
        api_update_entity_position((Entity *) player, &guid);
    }
}

// Handle
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const bool logged_in, std::string command) {
    command = from_cp437(command);
    if (!logged_in) {
        // Logged-Out
        if (command.starts_with(login_command)) {
            // Login
            command = command.substr(strlen(login_command));
            const std::string::size_type divider = command.find(' ');
            std::string username = command.substr(0, divider);
            const std::string password = command.substr(divider + 1);
            // Attempt
            username = to_cp437(username);
            misc_sanitize_username(username);
            const std::string username_utf = from_cp437(username);
            if (attempt_login(username_utf, password)) {
                // Success
                tell(self, guid, "Welcome, " + username_utf + '!');
                login(self, guid, username);
            } else {
                // Failure
                tell(self, guid, "Login Failed!");
            }
        }
        return true;
    } else {
        // Logged-In
        return false;
    }
}