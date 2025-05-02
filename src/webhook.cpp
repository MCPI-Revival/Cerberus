#include <fstream>
#include <sstream>
#include <unistd.h>

#include <libreborn/util/util.h>
#include <libreborn/util/exec.h>
#include <libreborn/util/string.h>
#include <libreborn/patch.h>

#include "mod.h"

// Get URL
static const std::string &get_url() {
    static std::string url;
    if (url.empty()) {
        // Read From Disk
        std::ifstream file (home_get() + "/webhook.txt", std::ios::binary);
        std::getline(file, url);
        if (url.empty()) {
            ERR("Unable To Read Webhook");
        }
        file.close();
    }
    return url;
}

// Make Webhook
static std::string escape(const std::string &input) {
    std::ostringstream escaped;
    for (const char c : input) {
        switch (c) {
            case '\"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b";  break;
            case '\f': escaped << "\\f";  break;
            case '\n': escaped << "\\n";  break;
            case '\r': escaped << "\\r";  break;
            case '\t': escaped << "\\t";  break;
            default: escaped << c;
        }
    }
    return escaped.str();
}
static std::string make_json(const std::string &message, const bool can_ping) {
    // Allowed Mentions
    std::string out = "{\"allowed_mentions\": {";
    out += '"' + std::string(discord_ping_is_user ? "users" : "roles") + "\": [";
    if (can_ping) {
        out += std::to_string(discord_ping_id);
    }
    out += ']';
    out += "}, ";
    // Suppress Embeds
    out += "\"flags\": 4, ";
    // Content
    out += "\"content\": \"";
    out += escape(message);
    out += '\"';
    // Return
    out += '}';
    return out;
}
// Send Message
static void redirect_file(FILE *file, const char *mode) {
    const FILE *ret = freopen("/dev/null", mode, file);
    if (!ret) {
        IMPOSSIBLE();
    }
}
void send_to_discord(const std::string &message, const bool can_ping) {
    // Get JSON
    std::string msg = message;
    if (can_ping) {
        msg = "<@" + std::to_string(discord_ping_id) + "> " + msg;
    }
    const std::string json = make_json(msg, can_ping);
    const std::string &url = get_url();
    // Send
    if (fork() == 0) {
        redirect_file(stdout, "w");
        redirect_file(stderr, "w");
        redirect_file(stdin, "r");
        const char *const argv[] = {
            "curl",
            "-X", "POST",
            "-H", "Content-Type: application/json",
            "-d", json.c_str(),
            url.c_str(),
            nullptr
        };
        safe_execvpe(argv, environ);
    }
}

// Logging
static void Gui_addMessage_injection(Gui_addMessage_t original, Gui *gui, const std::string &text) {
    static bool recursing = false;
    if (!recursing) {
        recursing = true;
        const std::string safe_message = from_cp437(text);
        send_to_discord(safe_message, false);
        original(gui, text);
        recursing = false;
    } else {
        original(gui, text);
    }
}

// Init
void init_webhook() {
    get_url();
    send_to_discord("**Server Started!**", false);
    // Logging
    overwrite_calls(Gui_addMessage, Gui_addMessage_injection);
}