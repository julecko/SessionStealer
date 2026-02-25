#include "sessionstealer/cli_args.h"
#include "dlls/discovery/discovery.h"

#include <stdio.h>
#include <stdbool.h>

static discovery_browser_name_t browser_map[] = {
    [CLI_BROWSER_CHROME]  = DISCOVERY_CHROME,
    [CLI_BROWSER_EDGE]    = DISCOVERY_EDGE,
    [CLI_BROWSER_FIREFOX] = DISCOVERY_FIREFOX
};

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
discovery_browser_t *pick_browser(discovery_browser_list_t *discovered_browsers, cli_args_browser_t args_browser) {
    if (discovered_browsers->length < 1) {
        printf("No browsers discovered\n");
    }

    if (!(args_browser == CLI_BROWSER_DETECT)) {
        discovery_browser_name_t target = browser_map[args_browser];

        for (size_t i = 0; i < discovered_browsers->length; i++) {
            discovery_browser_t *browser = &discovered_browsers->browsers[i];
            if (browser->browser_name == target) {
                return browser;
            }
        }
        printf("Selected browser was not found on this PC\n");
        return NULL;
    } else {
        printf("Discovered browsers:\n");
        for (size_t i = 0; i < discovered_browsers->length; i++) {
            discovery_browser_t *discovered_browser = &discovered_browsers->browsers[i];
            const char *browser_name = enum_browser_to_char(discovered_browser->browser_name);
            printf("    %d. %s\n", i+1, browser_name);
        }
        size_t input;
        while (true) {
            printf("Select browser number: ");
            if (scanf("%zu", &input) != 1) {
                printf("Invalid input! Enter number\n");

                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                continue;
            } else {
                if (input >= 1 && input <= discovered_browsers->length) {
                    break;
                } else {
                    printf("Select in range %zu-%zu\n", 1, discovered_browsers->length);
                }
            }
        }
        input--;
        return &discovered_browsers->browsers[input];
    }
}
