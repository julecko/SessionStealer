#ifndef DISCOVERY_UTILS_H
#define DISCOVERY_UTILS_H

#include <windows.h>
#include <stdbool.h>

bool file_exists(const char *path);
bool query_reg_path(const char *reg_path, char *outbuffer, DWORD outbuffer_size);
//const char *get_home_folder();

#endif
