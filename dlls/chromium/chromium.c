#include "chromium/chromium.h"
#include "chromium/util.h"
#include "dlls/discovery/discovery.h"
#include "chromium/fetch_cookies.h"
#include "chromium/load_cookies.h"

#include <stdio.h>
#include <stdbool.h>

#define WEBSOCKET_URL_MAX 67

// cppcheck-suppress unusedFunction
int import_browser(const discovery_browser_t *browser, const char *filepath) {
    run_program(5, "taskkill /F /IM msedge.exe");

    int port = 5000;
    char user_data_dir[MAX_PATH];
    get_user_data_dir(user_data_dir);

    run_program(0, "\"%s\" --remote-debugging-port=%d --remote-allow-origins=http://localhost:%d --user-data-dir=\"%s\" --profile-directory=\"Default\" \
                    --disable-logging --log-level=3",
                browser->exe_path, port, port, user_data_dir);

    char *initial_json = http_get_local_json(port);
    if (!initial_json) {
        fputs("Json from url was not returned\n", stderr);
        return 1;
    }

    char websocket_url[WEBSOCKET_URL_MAX] = {0};
    bool result = extract_ws_url(websocket_url, sizeof(websocket_url), initial_json);

    free(initial_json);
    if (!result) {
        fputs("Websocket url could not be extracted\n", stderr);
        return 1;
    }

    printf("%s\n", websocket_url);

    FILE *infile = fopen(filepath, "r");
    if (!infile) {
        printf("File not found\n");
        return 1;
    }
    load_cookies(websocket_url, infile);
    fclose(infile);

    run_program(0, "taskkill /F /IM msedge.exe");

    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser(const discovery_browser_t *browser, const char *filepath) {
    run_program(5, "taskkill /F /IM msedge.exe");

    int port = 5000;
    char user_data_dir[MAX_PATH];
    get_user_data_dir(user_data_dir);

    run_program(0, "\"%s\" --headless --disable-gpu --remote-debugging-port=%d --remote-allow-origins=http://localhost:%d --user-data-dir=\"%s\" --profile-directory=\"Default\" \
                    --disable-logging --log-level=3",
                browser->exe_path, port, port, user_data_dir);

    char *initial_json = http_get_local_json(port);
    if (!initial_json) {
        fputs("Json from url was not returned\n", stderr);
        return 1;
    }

    char websocket_url[WEBSOCKET_URL_MAX] = {0};
    bool result = extract_ws_url(websocket_url, sizeof(websocket_url), initial_json);

    free(initial_json);
    if (!result) {
        fputs("Websocket url could not be extracted\n", stderr);
        return 1;
    }

    printf("%s\n", websocket_url);

    FILE *outfile = fopen(filepath, "w");
    fetch_cookies(websocket_url, outfile);
    fclose(outfile);

    run_program(0, "taskkill /F /IM msedge.exe");

    return 0;
}
