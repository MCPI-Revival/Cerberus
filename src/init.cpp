#include "mod.h"

// Init
__attribute__((constructor)) static void init() {
    init_webhook();
    init_accounts();
    init_packets();
}