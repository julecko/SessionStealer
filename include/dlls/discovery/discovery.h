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

typedef enum {
    DISCOVERY_EDGE,
    DISCOVERY_CHROME,
    DISCOVERY_FIREFOX,
} discovery_browser_name_t;

typedef struct {
    discovery_browser_name_t browser_name;
    const char *exe_path;
} discovery_browser_t;

typedef struct {
    size_t length;
    size_t capacity;
    discovery_browser_t *discovered_browsers;
} discovery_browser_list_t;

DISCOVERY_API discovery_browser_list_t discover_browsers(void);

#endif
