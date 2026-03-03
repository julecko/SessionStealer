#include "shared/cookie_utils.h"
#include <stdint.h>

#define UNIX_TO_CHROMIUM_OFFSET 11644473600ULL

int64_t expires_firefox_to_chromium(int64_t firefox_expiry) {
    if (firefox_expiry == 0) return 0;
    return firefox_expiry / 1000LL;
}

int64_t expires_chromium_to_firefox(int64_t chromium_expiry) {
    if (chromium_expiry == 0) return 0;
    return chromium_expiry * 1000LL;
}
