#ifndef DLL_CHROMIUM_WEBSOCKET_H
#define DLL_CHROMIUM_WEBSOCKET_H

#include <windows.h>
#include <winhttp.h>
#include <stdbool.h>

#define WEBSOCKET_RECV_MAX 8192

HINTERNET connect_websocket(const char* ws_url);
void ws_send(HINTERNET ws, const char *c);
bool ws_recv(HINTERNET ws, char **out, bool *frame_finished);
void close_websocket(HINTERNET ws);

#endif
