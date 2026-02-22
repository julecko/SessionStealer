#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#define MAX_FILES_PARSED 256

typedef enum {
    CLI_ACTION_INVALID,
    CLI_ACTION_EXIT,
    CLI_ACTION_CONTINUE,
} cli_args_action_t;

typedef enum {
    CLI_FLAG_NONE = 0,
    CLI_FLAG_VERBOSE = 1 << 1,
} cli_args_flags_t;

typedef enum {
    CLI_COMMAND_IMPORT,
    CLI_COMMAND_EXPORT,
} cli_args_command_t;

typedef enum {
    CLI_BROWSER_INVALID,
    CLI_BROWSER_EDGE,
    CLI_BROWSER_CHROME,
    CLI_BROWSER_FIREFOX,
    CLI_BROWSER_DETECT,
} cli_args_browser_t;

typedef struct {
    const char *file;
    cli_args_flags_t flags;
    cli_args_browser_t browser;
    cli_args_command_t command;
} cli_args_t;

cli_args_t create_cli_args(void);
cli_args_action_t parse_cli_args(int argc, char *argv[], cli_args_t *args);

#endif