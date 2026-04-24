#ifndef SHARED_COOKIE_UTILS_H
#define SHARED_COOKIE_UTILS_H

#include <stdint.h>

int64_t expires_firefox_to_chromium(int64_t firefox_expiry);
int64_t expires_chromium_to_firefox(int64_t edge_expires);

#endif
