#ifndef SHARED_COOKIE_UTILS_H
#define SHARED_COOKIE_UTILS_H

#include <stdint.h>

uint64_t expires_firefox_to_chromium(uint64_t firefox_expiry);
uint64_t chromium_to_firefox(uint64_t chromium_expires);

#endif
