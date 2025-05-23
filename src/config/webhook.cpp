#include <fstream>
#include <sstream>
#include <unistd.h>

#include <libreborn/util/exec.h>
#include <libreborn/util/string.h>
#include <libreborn/patch.h>

#include "config.h"

// Configuration
struct Webhook final : ConfigFile {
    std::string url;
    std::string ping_type;
    snowflake ping_id;
    // Load/Save
    void clear() override {
        url = "";
    }
    void do_load(std::ifstream &) override;
    bool check_load() const override;
    bool can_save() const override {
        return false;
    }
    // Name
    const char *get_name() const override {
        return "Discord Webhook";
    }
    const char *get_file() const override {
        return "webhook.txt";
    }
};

// Get URL
void Webhook::do_load(std::ifstream &file) {
    // Load URL
    std::getline(file, url);
    // Load Ping Information
    std::string data;
    std::getline(file, data);
    const std::string::size_type i = data.find_first_of(':');
    if (i != std::string::npos) {
        ping_type = data.substr(0, i);
        const std::string id = data.substr(i + 1);
        ping_id = strtoull(id.c_str(), nullptr, 10);
    }
}
bool Webhook::check_load() const {
    return !url.empty() && !ping_type.empty();
}
static const Webhook &get_config() {
    static Webhook config;
    static bool loaded = false;
    if (!loaded) {
        config.load();
    }
    return config;
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
    const Webhook &config = get_config();
    // Allowed Mentions
    std::string out = "{\"allowed_mentions\": {";
    out += '"' + config.ping_type + "s\": [";
    if (can_ping) {
        out += config.ping_id;
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
    const Webhook &config = get_config();
    // Get JSON
    std::string msg = message;
    if (can_ping) {
        msg = "<@" + std::to_string(config.ping_id) + "> " + msg;
    }
    const std::string json = make_json(msg, can_ping);
    const std::string &url = config.url;
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
    get_config();
    send_to_discord("**Server Started!**", false);
    // Logging
    overwrite_calls(Gui_addMessage, Gui_addMessage_injection);
}