#include "dlls/discovery/discovery.h"

const char *enum_browser_to_char(discovery_browser_name_t e) {
    switch (e) {
    case DISCOVERY_CHROME:
        return "Chrome";
    case DISCOVERY_EDGE:
        return "Edge";
    case DISCOVERY_FIREFOX:
        return "Firefox";
    default:
        return NULL;
    }
}
