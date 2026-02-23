#include "discovery/discovery.h"
#include "discovery/edge.h"

#include <stdio.h>

discovery_browser_list_t discover_browsers(void) {
    discovery_browser_list_t browsers_list = {0};
    discovery_browser_t edge = discover_edge();
    return browsers_list;
}
