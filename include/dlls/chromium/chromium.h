#ifndef DLL_CHROMIUM_H
#define DLL_CHROMIUM_H

#include "dlls/discovery/discovery.h"

#ifdef _WIN32
    #ifdef CHROMIUM_EXPORTS
        #define CHROMIUM_API __declspec(dllexport)
    #else
        #define CHROMIUM_API __declspec(dllimport)
    #endif
#else
    #define CHROMIUM_API
#endif

typedef int (*browser_dll_fn)(const discovery_browser_t *browser, const char *filepath);

CHROMIUM_API int import_browser(const discovery_browser_t *browser, const char *filepath);
CHROMIUM_API int export_browser(const discovery_browser_t *browser, const char *filepath);

#endif
