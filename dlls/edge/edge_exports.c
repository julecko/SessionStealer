#include "dlls/edge/edge_exports.h"
#include "dlls/edge/edge.h"

EDGE_API int import_browser(const discovery_browser_t *browser, const char *filepath) {
    return import_browser_internal(browser, filepath);
}
EDGE_API int export_browser(const discovery_browser_t *browser, const char *filepath) {
    return export_browser_internal(browser, filepath);
}
