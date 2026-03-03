#include "shared/cookie_utils.h"
#include <stdint.h>

#define UNIX_TO_CHROMIUM_OFFSET 11644473600ULL

uint64_t expires_firefox_to_chromium(uint64_t firefox_expiry) {
    return (firefox_expiry + UNIX_TO_CHROMIUM_OFFSET) * 1000000ULL;
}

uint64_t expires_chromium_to_firefox(uint64_t chromium_expires) {
    if (chromium_expires == 0) return 0;
    return (uint64_t)((chromium_expires / 1000000ULL) - UNIX_TO_CHROMIUM_OFFSET);
}
