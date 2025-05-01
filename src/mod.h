#pragma once

#include <symbols/minecraft.h>

// Init
void init_webhook();

// Admin
typedef unsigned long long snowflake;
extern const snowflake discord_ping_id;
extern const bool discord_ping_is_user;
extern const std::string discord_admin;
bool is_admin(const ServerPlayer *player);

// Webhook
void send_to_discord(const std::string &message, bool can_ping);