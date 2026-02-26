#include "dlls/chromium/util.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <winhttp.h>
#include <stdbool.h>

#pragma comment(lib, "winhttp.lib")

#define CMDLINE_MAX 1024

static int run_command(const char *cmdline, DWORD wait_seconds) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    HANDLE hNull = CreateFileA(
        "NUL",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    si.hStdInput  = hNull;
    si.hStdOutput = hNull;
    si.hStdError  = hNull;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, (LPSTR)cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        DWORD err = GetLastError();
        fprintf(stderr, "Failed to run '%s', error %lu\n", cmdline, err);
        return -1;
    }

    int exit_code = -1;

    if (wait_seconds == 0) {
        exit_code = 0;
    } else {
        DWORD wait_ms = wait_seconds * 1000;
        DWORD wait_result = WaitForSingleObject(pi.hProcess, wait_ms);

        if (wait_result == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            fprintf(stderr, "Process timed out and was terminated\n");
            exit_code = -1;
        } else if (wait_result == WAIT_FAILED) {
            DWORD err = GetLastError();
            fprintf(stderr, "WaitForSingleObject failed: %lu\n", err);
            exit_code = -1;
        } else {
            DWORD code;
            if (GetExitCodeProcess(pi.hProcess, &code)) {
                exit_code = (int)code;
            }
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exit_code;
}

int run_program(DWORD wait_seconds, const char *fmt, ...) {
    char cmdline[CMDLINE_MAX];
    va_list args;
    va_start(args, fmt);
    vsnprintf(cmdline, sizeof(cmdline), fmt, args);
    va_end(args);

    printf("Running program: %s\n", cmdline);
    return run_command(cmdline, wait_seconds);
}

// Caller must free result
char *http_get_local_json(int port) {
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

bool extract_ws_url(char *buffer, size_t buffer_len, const char *json_text) {
    const char start_json_field_string[] = "webSocketDebuggerUrl";
    const char start_ws_string[] = "ws";
    const char end_char = '"';

    char *start_json_field = strstr(json_text, start_json_field_string);
    if (!start_json_field) {
        printf("%s not found\n", start_json_field_string);
        return false;
    }

    char *start_ws = strstr(start_json_field, start_ws_string);
    if (!start_ws) {
        printf("%s not found\n", start_ws_string);
        return false;
    }

    char *end = strchr(start_ws, end_char);
    if (!end) {
        printf("Ending %c not found\n", end_char);
    }

    size_t length = end - start_ws;
    if (length >= buffer_len) {
        return false;
    }

    memcpy(buffer, start_ws, length);
    buffer[length] = '\0';
    
    return true;
}

int get_user_data_dir(char *user_data_dir) {
    char localappdata[MAX_PATH];
    GetEnvironmentVariableA("LOCALAPPDATA", localappdata, MAX_PATH);
    return sprintf(user_data_dir, "%s\\Microsoft\\Edge\\User Data", localappdata);
}
