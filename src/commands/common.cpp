#include <mods/misc/misc.h>

#include "commands.h"

// Bullet Point
static constexpr const char *bullet = "• ";

// Load Commands
void add_common_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self) {
    // Generate Help
    commands.push_back({
        .name = "help",
        .callback = [&commands](MCPI_UNUSED const std::vector<std::string> &args) {
            std::vector<std::string> ret = {"All Commands:"};
            for (const Command &command : commands) {
                std::string line = bullet + command.name;
                for (const std::string &arg : command.args) {
                    line += " <" + arg + '>';
                }
                ret.push_back(line);
            }
            return ret;
        }
    });

    // List Players
    commands.push_back({
        .name = "list",
        .callback = [self](MCPI_UNUSED const std::vector<std::string> &args) {
            std::vector<std::string> ret = {"All Players:"};
            const Level *level = self->level;
            if (level) {
                for (const Player *other : level->players) {
                    ret.push_back(bullet + misc_get_player_username_utf(other));
                }
            }
            return ret;
        }
    });
}