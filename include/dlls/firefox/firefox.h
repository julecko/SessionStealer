#ifndef DLLS_FIREFOX_H
#define DLLS_FIREFOX_H

#include "dlls/discovery/discovery.h"

int import_browser_internal(const discovery_browser_t *browser, const char *filepath);
int export_browser_internal(const discovery_browser_t *browser, const char *filepath);

#endif
