#include "discovery/firefox.h"
#include "discovery/discovery.h"
#include "discovery/utils.h"

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#define FIREFOX_PATH_SYSTEM     "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
#define FIREFOX_PATH_SYSTEM_OLD "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"
#define FIREFOX_PATH_USER       "%s\\AppData\\Local\\Mozilla Firefox\\firefox.exe"
#define FIREFOX_PATH_REG        "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe"

bool discover_firefox(discovery_browser_t *browser) {
    browser->browser_name = DISCOVERY_FIREFOX;
    const char exe_path[MAX_PATH];

    bool result = query_reg_path(FIREFOX_PATH_REG, exe_path, sizeof(exe_path));
    if (!result && file_exists(exe_path)) {
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
