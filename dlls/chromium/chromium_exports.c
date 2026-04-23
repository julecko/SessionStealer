#include "dlls/chromium/chromium_exports.h"
#include "dlls/chromium/chromium.h"

CHROMIUM_API int import_browser(const discovery_browser_t *browser, const char *filepath);
CHROMIUM_API int export_browser(const discovery_browser_t *browser, const char *filepath);
