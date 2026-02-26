#ifndef DLL_CHROMIUM_WEBSOCKET_H
#define DLL_CHROMIUM_WEBSOCKET_H

#include <windows.h>
#include <winhttp.h>

HINTERNET connect_websocket(const char* ws_url);
void ws_send(HINTERNET ws, const char *c);
char* ws_recv(HINTERNET ws);
void close_websocket(HINTERNET ws);

#endif
