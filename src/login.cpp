#include <mods/api/api.h>
#include <libreborn/log.h>
#include <libreborn/util/string.h>

#include "mod.h"

// Login Player
void login(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, std::string username) {
    // Mark
    mark_logged_in(guid);

    // Update Username
    username = to_cp437(username);
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