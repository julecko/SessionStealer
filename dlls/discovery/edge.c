#include "discovery/edge.h"
#include "discovery/discovery.h"
#include "discovery/utils.h"

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#define EDGE_PATH_SYSTEM "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_SYSTEM_OLD "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_USER "%s\\AppData\\Local\\Microsoft\\Edge\\Application\\msedge.exe"
#define EDGE_PATH_REG "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe"

static const char *query_reg_path() {
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
            ) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return path;
        }

        RegCloseKey(hKey);
    } else {
        return NULL;
    }

    return path;
}

bool discover_edge(discovery_browser_t *browser) {
    browser->browser_name = DISCOVERY_EDGE;
    const char *exe_path;

    exe_path = query_reg_path();
    if (exe_path != NULL && file_exists(exe_path)) {
        strncpy(browser->exe_path, exe_path, MAX_PATH - 1);
        browser->exe_path[MAX_PATH - 1] = '\0';
        return true;
    }
    /*const char *home_path = get_home_folder();
    if (home_path) {
        printf("Home folder: %s\n", home_path);
    } else {
        printf("Failed to get home folder\n");
    }*/
    return false;
}
