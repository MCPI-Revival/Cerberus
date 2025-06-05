#include <fstream>
#include <unordered_set>
#include <mods/misc/misc.h>

#include "config.h"

// Configuration
struct Admins final : ConfigFile {
    std::unordered_set<std::string> data;
    // Load/Save
    void clear() override {
        data.clear();
    }
    void do_load(std::ifstream &) override;
    bool can_save() const override {
        return false;
    }
    // Name
    const char *get_name() const override {
        return "Administrator Accounts";
    }
    const char *get_file() const override {
        return "admins.txt";
    }
};

// Load
void Admins::do_load(std::ifstream &file) {
    std::string username;
    while (std::getline(file, username)) {
        data.insert(username);
    }
}

// Load
static Admins &get_admins() {
    static Admins admins;
    return admins;
}
void init_admins() {
    get_admins().load();
}

// Check
bool is_admin(const std::string &username_utf) {
    return get_admins().data.contains(username_utf);
}
bool is_admin(const Player *player) {
    return is_admin(misc_get_player_username_utf(player));
}
