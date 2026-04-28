#ifndef DLL_EDGE_EXPORTS_H
#define DLL_EDGE_EXPORTS_H

#include "dlls/discovery/discovery.h"

#ifdef WIN32
    #ifdef CHROME_EXPORTS
        #define CHROME_API __declspec(dllexport)
    #elif defined(CHROME_IMPORTS)
        #define CHROME_API __declspec(dllimport)
    #else
        #define CHROME_API
    #endif
#else
    #define CHROME_API
#endif

CHROME_API int import_browser(const discovery_browser_t *browser, const char *filepath);
CHROME_API int export_browser(const discovery_browser_t *browser, const char *filepath);

#endif
