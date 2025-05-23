#include <fstream>
#include <unordered_map>

#include "config.h"
#include "../mod.h"

// Manager
struct Accounts final : ConfigFile {
    std::unordered_map<std::string, std::string> data;
    // Load/Save
    void clear() override {
        data.clear();
    }
    void do_load(std::ifstream &) override;
    bool can_save() const override {
        return true;
    }
    void do_save(std::ofstream &) const override;
    // Name
    const char *get_name() const override {
        return "Accounts";
    }
    const char *get_file() const override {
        return "accounts.txt";
    }
};
static Accounts &get_accounts() {
    static Accounts accounts;
    return accounts;
}

// Load
void Accounts::do_load(std::ifstream &file) {
    while (true) {
        std::string username;
        if (!std::getline(file, username)) {
            break;
        }
        std::string password;
        std::getline(file, password);
        data[username] = password;
    }
}

// Save
void Accounts::do_save(std::ofstream &file) const {
    for (const std::pair<const std::string, std::string> &account : data) {
        file << account.first << std::endl;
        file << account.second << std::endl;
    }
}

// Discord Notification
static void notify(const std::string &type, const std::string &name) {
    send_to_discord("**" + type + ":** " + name, false);
}

// Create Account
bool create_account(const std::string &name, const std::string &password) {
    if (has_account(name)) {
        // Already Exists
        return false;
    }
    // Create
    get_accounts().data[name] = hash_password(password);
    get_accounts().save();
    notify("New Account Created", name);
    return true;
}

// Login
bool attempt_login(const std::string &name, const std::string &password) {
    if (!has_account(name)) {
        // Does Not Exist
        return false;
    }
    // Check
    return get_accounts().data.at(name) == hash_password(password);
}

// Delete
bool delete_account(const std::string &name) {
    if (!has_account(name)) {
        // Does Not Exist
        return false;
    }
    // Delete
    get_accounts().data.erase(name);
    get_accounts().save();
    notify("Banned", name);
    return true;
}

// Check
bool has_account(const std::string &name) {
    return get_accounts().data.contains(name);
}

// Change Password
bool change_password(const std::string &name, const std::string &old_password, const std::string &new_password) {
    // Check Old Password
    if (!attempt_login(name, old_password)) {
        // Incorrect Password
        return false;
    }
    // Change
    get_accounts().data[name] = hash_password(new_password);
    get_accounts().save();
    notify("Password Changed", name);
    return true;
}

// Init
void init_accounts() {
    get_accounts().load();
}