#include "discovery/edge.h"
#include "discovery/discovery.h"
#include "discovery/utils.h"

#include <stdio.h>
#include <windows.h>

#define EDGE_PATH_SYSTEM "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_SYSTEM_OLD "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_USER "%s\\AppData\\Local\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_REG "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe"

int file_exists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return 0;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return 0;
    return 1;
}

const char *query_reg_path() {
    HKEY hKey;
    static char path[MAX_PATH];
    DWORD size = sizeof(path);

    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        EDGE_PATH_REG,
        0,
        KEY_READ,
        &hKey
    );

    if (result == ERROR_SUCCESS) {
        if (RegQueryValueExA(
                hKey,
                NULL,
                NULL,
                NULL,
                (LPBYTE)path,
                &size
            ) == ERROR_SUCCESS)
        {
            printf("msedge path: %s\n", path);
        }

        RegCloseKey(hKey);
    } else {
        printf("Not found (%ld)\n", result);
        return NULL;
    }

    return path;
}

discovery_browser_t discover_edge() {
    discovery_browser_t discovered_browser = {0};
    const char *home_path = get_home_folder();
    query_reg_path();
    if (home_path) {
        printf("Home folder: %s\n", home_path);
    } else {
        printf("Failed to get home folder\n");
    }
    return discovered_browser;
}
