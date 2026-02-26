#include "chromium/chromium.h"
#include "dlls/discovery/discovery.h"

#include <stdio.h>

// cppcheck-suppress unusedFunction
int import_browser(const discovery_browser_t *browser, const char *filepath) {
    printf(browser->exe_path);
    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser(const discovery_browser_t *browser, const char *filepath) {
    printf(browser->exe_path);
    return 0;
}
