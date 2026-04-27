#include "dlls/chromium/util.h"
#include "dlls/discovery/discovery.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <winhttp.h>
#include <stdbool.h>

#pragma comment(lib, "winhttp.lib")


// Caller must free result
char *http_get_local_json_internal(int port) {
    HINTERNET hSession = WinHttpOpen(L"CDP/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        printf("[ERROR] WinHttpOpen failed: %lu\n", GetLastError());
        return NULL;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", port, 0);
    if (!hConnect) {
        printf("[ERROR] WinHttpConnect failed: %lu\n", GetLastError());
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/json",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        printf("[ERROR] WinHttpOpenRequest failed: %lu\n", GetLastError());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        printf("[ERROR] WinHttpSendRequest failed: %lu\n", GetLastError());
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        printf("[ERROR] WinHttpReceiveResponse failed: %lu\n", GetLastError());
    }

    DWORD size = 0;
    WinHttpQueryDataAvailable(hRequest, &size);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        printf("[ERROR] malloc failed\n");
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    DWORD read = 0;
    if (!WinHttpReadData(hRequest, buffer, size, &read)) {
        printf("[ERROR] WinHttpReadData failed: %lu\n", GetLastError());
        free(buffer);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }
    buffer[read] = 0;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return buffer;
}

bool extract_ws_url_internal(char *buffer, size_t buffer_len, const char *json_text) {
    const char start_json_field_string[] = "webSocketDebuggerUrl";
    const char start_ws_string[] = "ws";
    const char end_char = '"';

    char *start_json_field = strstr(json_text, start_json_field_string);
    if (!start_json_field) {
        printf("%s not found\n", start_json_field_string);
        return false;
    }

    const char *start_ws = strstr(start_json_field, start_ws_string);
    if (!start_ws) {
        printf("%s not found\n", start_ws_string);
        return false;
    }

    const char *end = strchr(start_ws, end_char);
    if (!end) {
        printf("Ending %c not found\n", end_char);
        return false;
    }

    size_t length = end - start_ws;
    if (length >= buffer_len) {
        return false;
    }

    memcpy(buffer, start_ws, length);
    buffer[length] = '\0';
    
    return true;
}

int get_user_data_dir_internal(discovery_browser_name_t browser, char *user_data_dir) {
    char localappdata[MAX_PATH];
    GetEnvironmentVariableA("LOCALAPPDATA", localappdata, MAX_PATH);

    switch (browser) {
        case DISCOVERY_EDGE:
            return sprintf(user_data_dir,
                "%s\\Microsoft\\Edge\\User Data",
                localappdata);

        case DISCOVERY_CHROME:
            return sprintf(user_data_dir,
                "%s\\Google\\Chrome\\User Data",
                localappdata);

        default:
            return 0;
    }
}
