#pragma once

#include <string>
#include <vector>

#include <symbols/minecraft.h>

// Configuration File
struct ConfigFile {
    virtual ~ConfigFile() = default;

    // Load/Save
    void save() const;
    virtual void load();

    // Name
    [[nodiscard]] virtual const char *get_name() const = 0;
    [[nodiscard]] virtual const char *get_file() const = 0;
    [[nodiscard]] std::string get_path() const;

    // Data
    virtual void clear() = 0;

    // Implementation
    [[nodiscard]] virtual bool can_save() const = 0;
    virtual void do_save(std::ofstream &) const;
    virtual void do_load(std::ifstream &) = 0;
    [[nodiscard]] virtual bool check_load() const;
};

// Init
void init_webhook();
void init_accounts();
void init_admins();

// Accounts
bool create_account(const std::string &name, const std::string &password);
bool attempt_login(const std::string &name, const std::string &password);
bool delete_account(const std::string &name);
bool has_account(const std::string &name);
bool change_password(const std::string &name, const std::string &old_password, const std::string &new_password);

// Webhook
typedef unsigned long long snowflake;
void send_to_discord(const std::string &message, bool can_ping);

// Admin
bool is_admin(const std::string &username_utf);
bool is_admin(const Player *player);

// Welcome Message
const std::vector<std::string> &get_welcome_messages();