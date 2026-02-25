#ifndef DLL_LOADER_H
#define DLL_LOADER_H

#include "dlls/discovery/discovery.h"

extern const char *browser_map_dll[];

const char *find_dll_browser(discovery_browser_name_t browser);

#endif
