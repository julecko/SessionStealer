#ifndef DLL_EDGE_UTIL_H
#define DLL_EDGE_UTIL_H

#include "dlls/discovery/discovery.h"
#include <windows.h>
#include <stdbool.h>

char *http_get_local_json(int port);
bool extract_ws_url(char *buffer, size_t buffer_len, const char *json_text);
int get_user_data_dir(discovery_browser_name_t browser, char *user_data_dir);

#endif
