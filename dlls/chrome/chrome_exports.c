#include "dlls/chrome/chrome_exports.h"
#include "dlls/chrome/chrome.h"

CHROME_API int import_browser(const discovery_browser_t *browser, const char *filepath) {
    return import_browser_internal(browser, filepath);
}

CHROME_API int export_browser(const discovery_browser_t *browser, const char *filepath) {
    return export_browser_internal(browser, filepath);
}
