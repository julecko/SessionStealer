#include "dlls/firefox/firefox.h"
#include "dlls/firefox/utils.h"
#include "dlls/firefox/fetch_cookies.h"
#include "dlls/firefox/load_cookies.h"
#include "dlls/discovery/discovery.h"
#include "shared/util.h"
#include "shared/cookies.h"

#include <stdio.h>
#include <stdbool.h>


// cppcheck-suppress unusedFunction
int import_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    char cookie_file[MAX_PATH] = {0};
    int len = get_cookie_file(cookie_file, sizeof(cookie_file));
    if (len <= 0) {
        printf("Cookie file for firefox not found\n");
        return 1;
    } else {
        printf("Cookies file found %s\n", cookie_file);
    }

    FILE *infile;
    if (fopen_s(&infile, filepath, "r") != 0) {
        printf("File not found\n");
        return 1;
    }

    load_cookies(cookie_file, infile);
    fclose(infile);

    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    char cookie_file[MAX_PATH] = {0};
    int len = get_cookie_file(cookie_file, sizeof(cookie_file));
    if (len <= 0) {
        printf("Cookie file for firefox not found\n");
        return 1;
    } else {
        printf("Cookies file found %s\n", cookie_file);
    }

    FILE *outfile;
    if (fopen_s(&outfile, filepath, "w") != 0) {
        fprintf(stderr, "Failed to open %s file\n", filepath);
        return 1;
    }

    fetch_cookies(cookie_file, outfile);
    fclose(outfile);

    return 0;
}
