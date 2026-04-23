#ifndef DLL_CHROMIUM_H
#define DLL_CHROMIUM_H

#include "dlls/discovery/discovery.h"

int import_browser(const discovery_browser_t *browser, const char *filepath);
int export_browser(const discovery_browser_t *browser, const char *filepath);

#endif
