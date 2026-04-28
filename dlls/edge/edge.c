#include "edge/edge.h"
#include "dlls/chromium/chromium_exports.h"
#include "dlls/discovery/discovery.h"
#include "edge/fetch_cookies.h"
#include "edge/load_cookies.h"
#include "shared/util.h"

#include <stdio.h>
#include <stdbool.h>

#define WEBSOCKET_URL_MAX 67

static HMODULE hChromium = NULL;
static http_get_local_json_fn http_get_local_json_ptr = NULL;
static extract_ws_url_fn extract_ws_url_ptr = NULL;
static get_user_data_dir_fn get_user_data_dir_ptr = NULL;
connect_websocket_fn connect_websocket_ptr = NULL;
ws_send_fn ws_send_ptr = NULL;
ws_recv_fn ws_recv_ptr = NULL;
close_websocket_fn close_websocket_ptr = NULL;

static void load_chromium_functions() {
    if (hChromium) return;

    hChromium = LoadLibraryA("chromium.dll");
    if (!hChromium) {
        printf("Failed to load chromium.dll\n");
        return;
    }

    http_get_local_json_ptr = (http_get_local_json_fn)GetProcAddress(hChromium, "http_get_local_json");
    extract_ws_url_ptr = (extract_ws_url_fn)GetProcAddress(hChromium, "extract_ws_url");
    get_user_data_dir_ptr = (get_user_data_dir_fn)GetProcAddress(hChromium, "get_user_data_dir");
    connect_websocket_ptr = (connect_websocket_fn)GetProcAddress(hChromium, "connect_websocket");
    ws_send_ptr = (ws_send_fn)GetProcAddress(hChromium, "ws_send");
    ws_recv_ptr = (ws_recv_fn)GetProcAddress(hChromium, "ws_recv");
    close_websocket_ptr = (close_websocket_fn)GetProcAddress(hChromium, "close_websocket");

    if (!http_get_local_json_ptr || !extract_ws_url_ptr || !get_user_data_dir_ptr ||
        !connect_websocket_ptr || !ws_send_ptr || !ws_recv_ptr || !close_websocket_ptr) {
        printf("Failed to load functions from chromium.dll\n");
        FreeLibrary(hChromium);
        hChromium = NULL;
    }
}

static const char *get_browser_process(discovery_browser_name_t browser) {
    switch (browser) {
        case DISCOVERY_EDGE:
            return "msedge.exe";
        case DISCOVERY_CHROME:
            return "chrome.exe";
        default:
            return NULL;
    }
}


// cppcheck-suppress unusedFunction
int import_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    load_chromium_functions();

    char cmd[128];
    sprintf_s(cmd, sizeof(cmd), "taskkill /F /IM %s", get_browser_process(browser->browser_name));
    
    run_program(1, "%s", cmd);

    int port = 5000;
    char user_data_dir[MAX_PATH];
    if (get_user_data_dir_ptr) {
        get_user_data_dir_ptr(browser->browser_name, user_data_dir, sizeof(user_data_dir));
    } else {
        printf("get_user_data_dir function not loaded\n");
        return 1;
    }

    run_program(0, "\"%s\" --remote-debugging-port=%d --remote-allow-origins=http://localhost:%d --user-data-dir=\"%s\" --profile-directory=\"Profile 2\" \
                    --disable-logging --log-level=3",
                browser->exe_path, port, port, user_data_dir);

    char *initial_json = http_get_local_json_ptr ? http_get_local_json_ptr(port) : NULL;
    if (!initial_json) {
        fputs("Json from url was not returned\n", stderr);
        return 1;
    }

    char websocket_url[WEBSOCKET_URL_MAX] = {0};
    bool result = extract_ws_url_ptr ? extract_ws_url_ptr(websocket_url, sizeof(websocket_url), initial_json) : false;

    free(initial_json);
    if (!result) {
        fputs("Websocket url could not be extracted\n", stderr);
        return 1;
    }

    printf("%s\n", websocket_url);

    FILE *infile;
    if (fopen_s(&infile, filepath, "r") != 0) {
        printf("File not found\n");
        return 1;
    }

    load_cookies(websocket_url, infile, false);
    fclose(infile);

    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    load_chromium_functions();

    char cmd[128];
    sprintf_s(cmd, sizeof(cmd), "taskkill /F /IM %s", get_browser_process(browser->browser_name));
    
    run_program(1, "%s", cmd);

    int port = 5000;
    char user_data_dir[MAX_PATH];
    if (get_user_data_dir_ptr) {
        get_user_data_dir_ptr(browser->browser_name, user_data_dir, sizeof(user_data_dir));
    } else {
        printf("get_user_data_dir function not loaded\n");
        return 1;
    }

    run_program(0, "\"%s\" --headless --disable-gpu --remote-debugging-port=%d --remote-allow-origins=http://localhost:%d --user-data-dir=\"%s\" --profile-directory=\"Default\" \
                    --disable-logging --log-level=3",
                browser->exe_path, port, port, user_data_dir);

    char *initial_json = http_get_local_json_ptr ? http_get_local_json_ptr(port) : NULL;
    if (!initial_json) {
        fputs("Json from url was not returned\n", stderr);
        return 1;
    }

    char websocket_url[WEBSOCKET_URL_MAX] = {0};
    bool result = extract_ws_url_ptr ? extract_ws_url_ptr(websocket_url, sizeof(websocket_url), initial_json) : false;

    free(initial_json);
    if (!result) {
        fputs("Websocket url could not be extracted\n", stderr);
        return 1;
    }

    printf("%s\n", websocket_url);

    FILE *outfile;
    if (fopen_s(&outfile, filepath, "w") != 0) {
        fprintf(stderr, "Couldnt open %s for writing\n", filepath);
        return 1;
    }
    fetch_cookies(websocket_url, outfile);
    fclose(outfile);

    run_program(0, "%s", cmd);

    return 0;
}
