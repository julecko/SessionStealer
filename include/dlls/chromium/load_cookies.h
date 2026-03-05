#ifndef DLLS_CHROMIUM_LOAD_COOKIES_H
#define DLLS_CHROMIUM_LOAD_COOKIES_H

#include <stdio.h>
#include <stdbool.h>

void load_cookies(const char *ws_url, const FILE *infile, bool json_only);

#endif
