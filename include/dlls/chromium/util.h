#ifndef DLL_CHROMIUM_UTIL_H
#define DLL_CHROMIUM_UTIL_H

#include <windows.h>
#include <stdbool.h>

int run_program(DWORD wait_seconds, const char *fmt, ...);
char *http_get_local_json(int port);
bool extract_ws_url(char *buffer, size_t buffer_len, const char *json_text);
int get_user_data_dir(char *user_data_dir);

#endif
