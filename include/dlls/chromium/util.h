#ifndef DLL_CHROMIUM_UTIL_H
#define DLL_CHROMIUM_UTIL_H

#include "dlls/chromium/chromium_exports.h"
#include "dlls/discovery/discovery.h"
#include <windows.h>
#include <stdbool.h>
#include <stddef.h>

CHROMIUM_API char *http_get_local_json_internal(int port);
CHROMIUM_API bool extract_ws_url_internal(char *buffer, size_t buffer_len, const char *json_text);
CHROMIUM_API int get_user_data_dir_internal(discovery_browser_name_t browser, char *user_data_dir, size_t user_data_dir_size);

#endif
