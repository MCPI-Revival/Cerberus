#pragma once

#include <symbols/minecraft.h>

// Init
void init_webhook();
void init_accounts();
void init_packets();

// Admin
typedef unsigned long long snowflake;
extern const snowflake discord_ping_id;
extern const bool discord_ping_is_user;
extern const std::string discord_admin;
bool is_admin(const ServerPlayer *player);

// Webhook
void send_to_discord(const std::string &message, bool can_ping);

// Hashing
std::string hash_password(const std::string &password);

// Accounts
bool create_account(const std::string &name, const std::string &password);
bool attempt_login(const std::string &name, const std::string &password);
bool delete_account(const std::string &name);

// Packets
void tell(const ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &message);
void mark_logged_in(const RakNet_RakNetGUID &guid);

// Commands
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, bool logged_in, std::string command);