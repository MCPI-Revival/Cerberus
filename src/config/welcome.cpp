#include <fstream>

#include <libreborn/util/string.h>

#include "config.h"

// Configuration
struct Welcome final : ConfigFile {
    std::vector<std::string> data;
    // Load/Save
    void clear() override {
        data.clear();
    }
    void do_load(std::ifstream &) override;
    bool check_load() const override;
    bool can_save() const override {
        return false;
    }
    // Name
    const char *get_name() const override {
        return "Welcome Message";
    }
    const char *get_file() const override {
        return "welcome.txt";
    }
};

// Load
void Welcome::do_load(std::ifstream &file) {
    std::string message;
    while (std::getline(file, message)) {
        trim(message);
        if (!message.empty()) {
            data.push_back(message);
        }
    }
}
bool Welcome::check_load() const {
    return !data.empty();
}

// Get
const std::vector<std::string> &get_welcome_messages() {
    // Load
    static Welcome welcome;
    static bool loaded = false;
    if (!loaded) {
        welcome.load();
        loaded = true;
    }
    // Return
    return welcome.data;
}