#include "sessionstealer/cli_args.h"
#include "sessionstealer/version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CLI_DEFAULT_FILE "cookies.csv"

static void print_help(FILE *file) {
    fputs("SessionStealer usage: sessionstealer [command] <flags>\n", file);
    fputs("Commands:\n", file);
    fputs("    import -> imports cookies into browser\n", file);
    fputs("    export -> extracts cookies from browser\n", file);
    fputs("Flags:\n", file);
    fputs("    -V/--version -> prints program version\n", file);
    fputs("    -h/--help -> showcase this help message\n", file);
    fputs("    -v/--verbose -> enable verbose output\n", file);
    fputs("    -f/--file (default cookies.csv) -> specify file used for cookie import/export\n", file);
    fputs("    -b/--browser -> selected browser, if not specify discover all possible and give options\n", file);
    fputs("Supported browsers:\n", file);
    fputs("    Edge\n", file);
    //fputs("    Chrome\n", file);
    //fputs("    Firefox\n", file);
    fputs("Examples:\n", file);
    fputs("    sessionstealer export -v -> exports cookies from later dicovered and selected browser into default file with verbose output\n", file);
    fputs("    sessionstealer import -f cookies.csv -b edge -> import cookies from cookies.csv into edge\n", file);
    fputs("    sessionstealer export -f cookies.csv -> export cookies from later discovered and selected browser into file cookies.csv\n", file);
    fputs("    sessionstealer -h -> shows this help message\n", file);
    fputs("    sessionstealer -V -> prints version of this program\n", file);
}

cli_args_t create_cli_args(void) {
    cli_args_t args = {0};
    args.file = CLI_DEFAULT_FILE;
    args.browser = CLI_BROWSER_DETECT;
    args.flags = CLI_FLAG_NONE;

    return args;
}

static int my_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        unsigned char c1 = (unsigned char)*s1;
        unsigned char c2 = (unsigned char)*s2;

        c1 = (unsigned char)tolower(c1);
        c2 = (unsigned char)tolower(c2);

        if (c1 != c2)
            return c1 - c2;

        s1++;
        s2++;
    }

    return (unsigned char)tolower((unsigned char)*s1) -
           (unsigned char)tolower((unsigned char)*s2);
}

static cli_args_browser_t convert_browser(const char *browser) {
    cli_args_browser_t converted_browser = CLI_BROWSER_INVALID;

    if (my_strcasecmp(browser, "edge") == 0) {
        converted_browser = CLI_BROWSER_EDGE;
    } else if (my_strcasecmp(browser, "firefox") == 0) {
        converted_browser = CLI_BROWSER_FIREFOX;
    } else if (my_strcasecmp(browser, "chrome") == 0) {
        converted_browser = CLI_BROWSER_CHROME;
    }

    return converted_browser;
}

cli_args_action_t parse_cli_args(int argc, char *argv[], cli_args_t *args) {
    cli_args_action_t action = CLI_ACTION_INVALID;

    if (argc < 2) {
        print_help(stdout);
        return CLI_ACTION_EXIT;
    }

    const char *arg = argv[1];
    if (strcmp(arg, "import") == 0) {
        action = CLI_ACTION_CONTINUE;
        args->command = CLI_COMMAND_IMPORT;
    } else if (strcmp(arg, "export") == 0) {
        action = CLI_ACTION_CONTINUE;
        args->command = CLI_COMMAND_EXPORT;
    } else if (strcmp(arg, "-V") == 0 || strcmp(arg, "--version") == 0) {
        puts("SessionStealer version: " PROJECT_VERSION);
        return CLI_ACTION_EXIT;
    } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
        print_help(stdout);
        return CLI_ACTION_EXIT;
    } else {
        fputs("Command not specified\n", stderr);
        print_help(stderr);
        return action;
    }

    for (int i = 2; i < argc; i++) {
        arg = argv[i];
        if (strcmp(arg, "-f") == 0) {
            if (++i == argc) {
                fputs("Flag -f requires additional argument\n", stderr);
                return CLI_ACTION_EXIT;
            }
            args->file = argv[i];
        } else if (strcmp(arg, "-b") == 0) {
            if (++i == argc) {
                fputs("Flag -b requires additional argument\n", stderr);
                return CLI_ACTION_EXIT;
            }
            args->browser = convert_browser(argv[i]);
            if (args->browser == CLI_BROWSER_INVALID) {
                fputs("Invalid browser\n", stderr);
                return CLI_ACTION_INVALID;
            }
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0){
            args->flags |= CLI_FLAG_VERBOSE;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", arg);
            return CLI_ACTION_INVALID;
        }
    }

    return action; 
}
