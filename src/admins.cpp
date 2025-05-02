#include <unordered_set>

#include "mod.h"

// Who To Ping In Logs
const snowflake discord_ping_id = 385604875179786241;
const bool discord_ping_is_user = true;

// Who To Specify In Chat Messages
const std::string discord_admin = "TheBrokenRail";

// Players
static std::unordered_set admins = {
    discord_admin
};
bool is_admin(const Player *player) {
    return admins.contains(player->username);
}