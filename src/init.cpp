#include <libreborn/config.h>
#include <libreborn/log.h>

#include "mod.h"
#include "config/config.h"

// Init
__attribute__((constructor)) static void init() {
    if (!reborn_is_server()) {
        IMPOSSIBLE();
    }
    init_accounts();
    init_packets();
    init_admins();
    get_welcome_messages();
    init_webhook();
}