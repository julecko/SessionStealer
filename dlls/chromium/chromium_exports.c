#include "dlls/chromium/chromium_exports.h"
#include "dlls/chromium/util.h"
#include "dlls/chromium/websocket.h"
#include <stdbool.h>
#include <windows.h>
#include <winhttp.h>
#include <stddef.h>

CHROMIUM_API char *http_get_local_json(int port) {
    return http_get_local_json_internal(port);
}

CHROMIUM_API bool extract_ws_url(char *buffer, size_t buffer_len, const char *json_text) {
    return extract_ws_url_internal(buffer, buffer_len, json_text);
}

CHROMIUM_API int get_user_data_dir(discovery_browser_name_t browser, char *user_data_dir, size_t user_data_dir_size) {
    return get_user_data_dir_internal(browser, user_data_dir, user_data_dir_size);
}

CHROMIUM_API HINTERNET connect_websocket(const char* ws_url) {
    return connect_websocket_internal(ws_url);
}

CHROMIUM_API void ws_send(HINTERNET ws, const char *c) {
    ws_send_internal(ws, c);
}

CHROMIUM_API bool ws_recv(HINTERNET ws, char **out, bool *frame_finished) {
    return ws_recv_internal(ws, out, frame_finished);
}

CHROMIUM_API void close_websocket(HINTERNET ws) {
    close_websocket_internal(ws);
}