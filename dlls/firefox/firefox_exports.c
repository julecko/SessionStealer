#include "dlls/firefox/firefox_exports.h"
#include "dlls/firefox/firefox.h"

FIREFOX_API int import_browser(const discovery_browser_t *browser, const char *filepath) {
    return import_browser_internal(browser, filepath);
}
FIREFOX_API int export_browser(const discovery_browser_t *browser, const char *filepath) {
    return export_browser_internal(browser, filepath);
}
