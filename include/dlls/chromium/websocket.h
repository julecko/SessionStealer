#ifndef DLL_CHROMIUM_WEBSOCKET_H
#define DLL_CHROMIUM_WEBSOCKET_H

#include "dlls/chromium/chromium_exports.h"
#include <windows.h>
#include <winhttp.h>
#include <stdbool.h>

#define WEBSOCKET_RECV_MAX 8192

CHROMIUM_API HINTERNET connect_websocket(const char* ws_url);
CHROMIUM_API void ws_send(HINTERNET ws, const char *c);
CHROMIUM_API bool ws_recv(HINTERNET ws, char **out, bool *frame_finished);
CHROMIUM_API void close_websocket(HINTERNET ws);

#endif
