#ifndef DLL_CHROMIUM_FETCH_COOKIES_H
#define DLL_CHROMIUM_FETCH_COOKIES_H

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

void fetch_cookies(const char *ws_url, const FILE *outfile);

#endif
