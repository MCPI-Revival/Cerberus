#include <libreborn/config.h>
#include <libreborn/log.h>

#include "mod.h"

// Init
__attribute__((constructor)) static void init() {
    if (!reborn_is_server()) {
        IMPOSSIBLE();
    }
    init_webhook();
    init_accounts();
    init_packets();
}