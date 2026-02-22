#include "sessionstealer/version.h"
#include "sessionstealer/cli_args.h"

#include <stdlib.h>
#include <stdio.h>
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

    int (*discover_func)() = (int (*)())GetProcAddress(hLib, "discover_browsers");
    if (!discover_func) {
        printf("Failed to find discover_browsers in DLL\n");
        FreeLibrary(hLib);
        return 1;
    }

    int result = discover_func();
    printf("discover_browsers returned %d\n", result);

    FreeLibrary(hLib);
    return 0;


    printf("Project version: %s\n", PROJECT_VERSION);
    return 0;
}
