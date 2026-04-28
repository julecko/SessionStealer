#ifndef DLL_CHROMIUM_EXPORTS_H
#define DLL_CHROMIUM_EXPORTS_H

#include "dlls/discovery/discovery.h"
#include <windows.h>
#include <winhttp.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef CHROMIUM_EXPORTS
    #define CHROMIUM_API __declspec(dllexport)
#elif defined(CHROMIUM_IMPORTS)
    #define CHROMIUM_API __declspec(dllimport)
#else
    #define CHROMIUM_API
#endif

typedef char* (*http_get_local_json_fn)(int);
typedef bool (*extract_ws_url_fn)(char*, size_t, const char*);
typedef int (*get_user_data_dir_fn)(int, char*, size_t);
typedef HINTERNET (*connect_websocket_fn)(const char*);
typedef void (*ws_send_fn)(HINTERNET, const char*);
typedef bool (*ws_recv_fn)(HINTERNET, char**, bool*);
typedef void (*close_websocket_fn)(HINTERNET);

CHROMIUM_API char *http_get_local_json(int port);
CHROMIUM_API bool extract_ws_url(char *buffer, size_t buffer_len, const char *json_text);
CHROMIUM_API int get_user_data_dir(discovery_browser_name_t browser, char *user_data_dir, size_t user_data_dir_size);
CHROMIUM_API HINTERNET connect_websocket(const char* ws_url);
CHROMIUM_API void ws_send(HINTERNET ws, const char *c);
CHROMIUM_API bool ws_recv(HINTERNET ws, char **out, bool *frame_finished);
CHROMIUM_API void close_websocket(HINTERNET ws);

#endif
