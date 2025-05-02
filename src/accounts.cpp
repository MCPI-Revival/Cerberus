#include <fstream>
#include <unordered_map>

#include <libreborn/log.h>
#include <libreborn/util/util.h>

#include "mod.h"

// Manager
struct Accounts {
    std::unordered_map<std::string, std::string> data;
    void load();
    void save() const;
};
static Accounts &get_accounts() {
    static Accounts accounts;
    return accounts;
}

// Load
static std::string get_path() {
    return home_get() + "/accounts.txt";
}
void Accounts::load() {
    data.clear();
    std::ifstream file(get_path(), std::ios::binary);
    while (file && file.peek() != EOF) {
        std::string username;
        std::getline(file, username);
        std::string password;
        std::getline(file, password);
        data[username] = password;
    }
    file.close();
    save();
}
void Accounts::save() const {
    std::ofstream file(get_path(), std::ios::binary);
    for (const std::pair<const std::string, std::string> &account : data) {
        file << account.first << std::endl;
        file << account.second << std::endl;
    }
    file.close();
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
    send_to_discord("**New Account Created:** " + name, false);
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
    send_to_discord("**Banned:** " + name, false);
    return true;
}

// Check
bool has_account(const std::string &name) {
    return get_accounts().data.contains(name);
}

// Init
void init_accounts() {
    get_accounts().load();
}