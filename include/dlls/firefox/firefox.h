#ifndef DLLS_FIREFOX_H
#define DLLS_FIREFOX_H

#include "dlls/discovery/discovery.h"

#ifdef _WIN32
    #ifdef FIREFOX_EXPORTS
        #define FIREFOX_API __declspec(dllexport)
    #elif defined(FIREFOX_IMPORTS)
        #define FIREFOX_API __declspec(dllimport)
    #else
        #define FIREFOX_API
    #endif
#else
    #define FIREFOX_API
#endif

FIREFOX_API int import_browser(const discovery_browser_t *browser, const char *filepath);
FIREFOX_API int export_browser(const discovery_browser_t *browser, const char *filepath);

#endif
