#include "version.h"
#include "cli_args.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    cli_args_t args = create_cli_args();
    cli_args_action_t action = parse_cli_args(argc, argv, &args);

    if (action == CLI_ACTION_EXIT)
        return EXIT_SUCCESS;
    if (action == CLI_ACTION_INVALID)
        return EXIT_FAILURE;


    printf("Project version: %s\n", PROJECT_VERSION);
    return 0;
}
