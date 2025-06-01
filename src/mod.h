#pragma once

#include <symbols/minecraft.h>

// Init
void init_packets();

// Hashing
std::string hash_password(const std::string &password);
bool hash_check(const std::string &password, const std::string &hash);

// Packets
void tell(const ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &message /* In UTF-8 */);
void mark_logged_in(const RakNet_RakNetGUID &guid);

// Commands
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, bool logged_in, std::string command /* In CP-437 */);

// Login
void login(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, std::string username /* In UTF-8 */);