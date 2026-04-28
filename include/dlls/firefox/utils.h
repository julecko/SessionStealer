#ifndef DLLS_FIREFOX_UTILS_H
#define DLLS_FIREFOX_UTILS_H

#include <stddef.h>

void normalize_ini_path(char *path);
int get_cookie_file(char *out, size_t out_size);


#endif
