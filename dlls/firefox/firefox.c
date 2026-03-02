#include "chromium/chromium.h"
#include "chromium/util.h"
#include "dlls/discovery/discovery.h"
#include "chromium/fetch_cookies.h"
#include "chromium/load_cookies.h"

#include <stdio.h>
#include <stdbool.h>

#define WEBSOCKET_URL_MAX 67

// cppcheck-suppress unusedFunction
int import_browser(const discovery_browser_t *browser, const char *filepath) {

    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser(const discovery_browser_t *browser, const char *filepath) {

    printf("%s\n", browser->exe_path);

    return 0;
}
