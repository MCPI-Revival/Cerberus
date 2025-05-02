#include <unordered_set>

#include <libreborn/patch.h>
#include <libreborn/util/string.h>
#include <libreborn/util/util.h>

#include <mods/chat/chat.h>

#include "mod.h"

// Track Logged-In Users
struct GUID_Helper {
    struct Hash {
        size_t operator()(const RakNet_RakNetGUID &guid) const {
            return RakNet_RakNetGUID::ToUint32(guid);
        }
    };
    struct Equals {
        bool operator()(const RakNet_RakNetGUID &a, const RakNet_RakNetGUID &b) const {
            return const_cast<RakNet_RakNetGUID &>(a).equals(b);
        }
    };
};
static std::unordered_set<RakNet_RakNetGUID, GUID_Helper::Hash, GUID_Helper::Equals> logged_in;
static void ServerSideNetworkHandler_onDisconnect_injection(ServerSideNetworkHandler_onDisconnect_t original, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid) {
    // Log-Out
    logged_in.erase(guid);
    // Call Original Method
    original(self, guid);
}
void mark_logged_in(const RakNet_RakNetGUID &guid) {
    logged_in.insert(guid);
}

// Limit Packets For Unlogged Users
static bool is_packet_safe(const Packet *packet) {
    static std::unordered_set allowed = {
        (void *) LoginPacket_vtable::base,
        (void *) RequestChunkPacket_vtable::base,
        (void *) ChatPacket_vtable::base,
        (void *) ReadyPacket_vtable::base
    };
    return allowed.contains((void *) packet->vtable);
}
static void RakNetInstance_runEvents_Packet_handle_injection(Packet *self, const RakNet_RakNetGUID &guid, NetEventCallback *callback) {
    bool allowed = true;
    if (!logged_in.contains(guid) && !is_packet_safe(self)) {
        allowed = false;
    }
    if (allowed) {
        self->handle(guid, callback);
    }
}

// Initially Place Player Out-Of-Bounds
static void ServerSideNetworkHandler_handle_LoginPacket_StartGamePacket_write_injection(StartGamePacket *self, RakNet_BitStream *stream) {
    // Update Position
    self->x = self->z = 65536;
    self->y = 128;
    // Call Original Method
    self->write(stream);
}

// Handle Connection
void tell(const ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &message) {
    MessagePacket *packet = MessagePacket::allocate();
    ((Packet *) packet)->constructor();
    packet->vtable = MessagePacket_vtable::base;
    packet->message.constructor();
    packet->message.Assign(to_cp437(message).c_str());
    packet->reliability = RELIABLE_ORDERED;
    self->rak_net_instance->sendTo(guid, *(Packet *) packet);
}
static void ServerSideNetworkHandler_onReady_ClientGeneration_injection(ServerSideNetworkHandler_onReady_ClientGeneration_t original, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid) {
    // Handle Logged-In Users
    if (logged_in.contains(guid)) {
        original(self, guid);
        return;
    }
    // Send Welcome Message
    tell(self, guid, "Welcome to the official MCPI-Revival server!");
    tell(self, guid, "To login, chat \"/login <username> <password>\".");
    tell(self, guid, "If you do not have an account, DM @" + discord_admin + " on Discord.");
}

// Handle Chat Commands
HOOK(ServerSideNetworkHandler_handle_ChatPacket_injection, void, (ServerSideNetworkHandler *server_side_network_handler, const RakNet_RakNetGUID &guid, ChatPacket *chat_packet)) {
    const bool is_logged_in = logged_in.contains(guid);
    const bool ret = handle_command(server_side_network_handler, guid, is_logged_in, chat_packet->message);
    if (is_logged_in && !ret) {
        // Call Original Method
        real_ServerSideNetworkHandler_handle_ChatPacket_injection()(server_side_network_handler, guid, chat_packet);
    }
}

// Init
void init_packets() {
    // Limit Packets
    overwrite_call((void *) 0x740d0, Packet_handle, RakNetInstance_runEvents_Packet_handle_injection);
    // Update Player Position
    overwrite_call((void *) 0x764f4, StartGamePacket_write, ServerSideNetworkHandler_handle_LoginPacket_StartGamePacket_write_injection);
    // Welcome Message
    overwrite_calls(ServerSideNetworkHandler_onReady_ClientGeneration, ServerSideNetworkHandler_onReady_ClientGeneration_injection);
    // Handle Disconnect
    overwrite_calls(ServerSideNetworkHandler_onDisconnect, ServerSideNetworkHandler_onDisconnect_injection);
}