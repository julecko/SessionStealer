#ifndef DLL_EDGE_FETCH_COOKIES_H
#define DLL_EDGE_FETCH_COOKIES_H

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

void fetch_cookies(const char *ws_url, FILE *outfile);

#endif
