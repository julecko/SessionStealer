#ifndef DLL_EDGE_EXPORTS_H
#define DLL_EDGE_EXPORTS_H

#include "dlls/discovery/discovery.h"

#ifdef _WIN32
    #ifdef EDGE_EXPORTS
        #define EDGE_API __declspec(dllexport)
    #else
        #define EDGE_API __declspec(dllimport)
    #endif
#else
    #define EDGE_API
#endif

EDGE_API int import_browser(const discovery_browser_t *browser, const char *filepath);
EDGE_API int export_browser(const discovery_browser_t *browser, const char *filepath);

#endif
