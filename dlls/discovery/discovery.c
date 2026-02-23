#include "discovery/discovery.h"
#include "discovery/edge.h"

#include <stdio.h>
#include <stdbool.h>

void create_browser_list() {
    // TODO init to static memory fixed size of browsers
}

discovery_browser_list_t discover_browsers(void) {
    discovery_browser_list_t browsers_list = {0};
    discovery_browser_t edge = {0};
    bool result = discover_edge(&edge);
    return browsers_list;
}
