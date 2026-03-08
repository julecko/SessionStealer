#include "discovery/chrome.h"
#include "discovery/discovery.h"
#include "discovery/utils.h"

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#define CHROME_PATH_SYSTEM "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"
#define CHROME_PATH_SYSTEM_X86 "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"
#define CHROME_PATH_USER "%s\\AppData\\Local\\Google\\Chrome\\Application\\chrome.exe"
#define CHROME_PATH_REG "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe"

bool discover_chrome(discovery_browser_t *browser) {
    browser->browser_name = DISCOVERY_CHROME;

    char exe_path[MAX_PATH];

    bool result = query_reg_path(CHROME_PATH_REG, exe_path, sizeof(exe_path));
    if (result && file_exists(exe_path)) {
        strncpy(browser->exe_path, exe_path, MAX_PATH - 1);
        browser->exe_path[MAX_PATH - 1] = '\0';
        return true;
    }

    if (file_exists(CHROME_PATH_SYSTEM)) {
        strncpy(browser->exe_path, CHROME_PATH_SYSTEM, MAX_PATH - 1);
        browser->exe_path[MAX_PATH - 1] = '\0';
        return true;
    }

    if (file_exists(CHROME_PATH_SYSTEM_X86)) {
        strncpy(browser->exe_path, CHROME_PATH_SYSTEM_X86, MAX_PATH - 1);
        browser->exe_path[MAX_PATH - 1] = '\0';
        return true;
    }

    return false;
}
