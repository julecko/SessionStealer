#include "dlls/chrome/chrome.h"
#include "dlls/chromium/chromium_exports.h"
#include "dlls/discovery/discovery.h"
#include "dlls/chrome/fetch_cookies.h"
#include "dlls/chrome/load_cookies.h"
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

static int get_websocket_url(const discovery_browser_t *browser,
                             int port,
                             const char *extra_args,
                             char *out_ws,
                             size_t out_size) {

    if (!get_user_data_dir_ptr || !http_get_local_json_ptr || !extract_ws_url_ptr) {
        fprintf(stderr, "Chromium functions not loaded\n");
        return 1;
    }

    char cmd[128];
    sprintf_s(cmd, sizeof(cmd), "taskkill /F /IM %s", "chrome.exe");
    run_program(1, "%s", cmd);

    char user_data_dir[MAX_PATH];
    get_user_data_dir_ptr(browser->browser_name, user_data_dir, sizeof(user_data_dir));

    run_program(0,
        "\"%s\" %s --remote-debugging-port=%d "
        "--remote-allow-origins=http://localhost:%d "
        "--user-data-dir=\"%s\" "
        "--disable-logging --log-level=3",
        browser->exe_path,
        extra_args,
        port, port,
        user_data_dir
    );

    char *json = http_get_local_json_ptr(port);
    if (!json) {
        fputs("Json not returned\n", stderr);
        return 1;
    }

    bool ok = extract_ws_url_ptr(out_ws, out_size, json);
    free(json);

    if (!ok) {
        fputs("Failed to extract websocket URL\n", stderr);
        return 1;
    }

    return 0;
}

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

// cppcheck-suppress unusedFunction
int import_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    load_chromium_functions();
    char ws[WEBSOCKET_URL_MAX] = {0};

    if (get_websocket_url(browser, 5000,
        "--profile-directory=\"Profile 2\"",
        ws, sizeof(ws)))
        return 1;

    FILE *infile;
    if (fopen_s(&infile, filepath, "r") != 0) {
        fprintf(stderr, "File not found\n");
        return 1;
    }

    load_cookies(ws, infile, false);
    fclose(infile);

    return 0;
}

// cppcheck-suppress unusedFunction
int export_browser_internal(const discovery_browser_t *browser, const char *filepath) {
    load_chromium_functions();
    char ws[WEBSOCKET_URL_MAX] = {0};

    if (get_websocket_url(browser, 5000,
        "--headless --disable-gpu --profile-directory=\"Default\"",
        ws, sizeof(ws)))
        return 1;

    FILE *outfile;
    if (fopen_s(&outfile, filepath, "w") != 0) {
        fprintf(stderr, "Couldn't open %s\n", filepath);
        return 1;
    }

    fetch_cookies(ws, outfile);
    fclose(outfile);

    return 0;
}
