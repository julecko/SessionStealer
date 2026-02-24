#include "discovery/discovery.h"
#include "discovery/edge.h"

#include <stdio.h>
#include <stdbool.h>

// cppcheck-suppress unusedFunction
discovery_browser_list_t discover_browsers(void) {
    discovery_browser_list_t browsers_list = {0};
    discovery_browser_t edge;
    
    bool result = discover_edge(&edge);
    if (result) {
        if (browsers_list.length < DISCOVERY_BROWSER_COUNT) {
            browsers_list.browsers[browsers_list.length] = edge;
            browsers_list.length++;
        }
    }
    return browsers_list;
}
