#ifndef DISCOVERY_DISCOVERY_H
#define DISCOVERY_DISCOVERY_H

#ifdef _WIN32
    #ifdef DISCOVERY_EXPORTS
        #define DISCOVERY_API __declspec(dllexport)
    #else
        #define DISCOVERY_API __declspec(dllimport)
    #endif
#else
    #define DISCOVERY_API
#endif

#include <windows.h>

typedef enum {
    DISCOVERY_EDGE,
    DISCOVERY_CHROME,
    DISCOVERY_FIREFOX,
    DISCOVERY_BROWSER_COUNT,
} discovery_browser_name_t;

typedef struct {
    discovery_browser_name_t browser_name;
    char exe_path[MAX_PATH];
} discovery_browser_t;

typedef struct {
    size_t length;
    discovery_browser_t browsers[DISCOVERY_BROWSER_COUNT];
} discovery_browser_list_t;

typedef discovery_browser_list_t (*discover_browsers_fn)(void);
DISCOVERY_API discovery_browser_list_t discover_browsers(void);

#endif
