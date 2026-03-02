#include "discovery/discovery.h"
#include "discovery/edge.h"
#include "discovery/firefox.h"

#include <stdio.h>
#include <stdbool.h>

typedef bool (*discover_func_t)(discovery_browser_t *browser);

static discover_func_t browser_discover_table[] = {
    discover_edge,
    discover_firefox,
};

// cppcheck-suppress unusedFunction
discovery_browser_list_t discover_browsers(void) {
    discovery_browser_list_t browsers_list = {0};

    size_t table_size = sizeof(browser_discover_table) / sizeof(browser_discover_table[0]);
    for (size_t i = 0; i < table_size; i++) {
        discovery_browser_t browser = {0};
        if (browser_discover_table[i](&browser)) {
            if (browsers_list.length < DISCOVERY_BROWSER_COUNT) {
                browsers_list.browsers[browsers_list.length++] = browser;
            }
        }
    }

    return browsers_list;
}
