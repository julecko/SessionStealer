#ifndef DLL_EDGE_H
#define DLL_EDGE_H

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


EDGE_API int import_browser(discovery_browser_t *browser);
EDGE_API int export_browser(discovery_browser_t *browser);

#endif
