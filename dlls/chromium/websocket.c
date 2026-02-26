#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

// Must free result
static wchar_t* utf8_to_wide(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t* out = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, out, len);
    return out;
}

HINTERNET connect_websocket(const char* ws_url) {
    wchar_t* wurl_original = utf8_to_wide(ws_url);
    wchar_t ws_fixed[512];

    if (wcsncmp(wurl_original, L"ws://", 5) == 0) {
        swprintf(ws_fixed, 512, L"http://%ls", wurl_original + 5);
    } 
    else if (wcsncmp(wurl_original, L"wss://", 6) == 0) {
        swprintf(ws_fixed, 512, L"https://%ls", wurl_original + 6);
    } 
    else {
        wcsncpy(ws_fixed, wurl_original, 511);
        ws_fixed[511] = L'\0';
    }

    free(wurl_original);


    URL_COMPONENTSW parts;
    ZeroMemory(&parts, sizeof(parts));
    parts.dwStructSize = sizeof(parts);

    wchar_t host[256];
    wchar_t path[512];

    parts.lpszHostName = host;
    parts.dwHostNameLength = 256;
    parts.lpszUrlPath = path;
    parts.dwUrlPathLength = 512;

    if (!WinHttpCrackUrl(ws_fixed, 0, 0, &parts)) {
        DWORD err = GetLastError();

        LPSTR msg = NULL;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            0,
            (LPSTR)&msg,
            0,
            NULL);

        printf("WinHttpCrackUrl failed. Error %lu: %s\n", err, msg);

        LocalFree(msg);
        return NULL;
    }

    HINTERNET session = WinHttpOpen(
        L"CDP/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);

    HINTERNET connect = WinHttpConnect(
        session,
        host,
        parts.nPort,
        0);

    HINTERNET request = WinHttpOpenRequest(
        connect,
        L"GET",
        path,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);

    WinHttpSetOption(
        request,
        WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET,
        NULL,
        0);

    if (!WinHttpSendRequest(request,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        NULL,
        0,
        0,
        0)) {
        printf("WinHttpSendRequest failed: %lu\n", GetLastError());
        return NULL;
    }

    if (!WinHttpReceiveResponse(request, NULL)) {
        printf("WinHttpReceiveResponse failed: %lu\n", GetLastError());
        return NULL;
    }

    HINTERNET ws = WinHttpWebSocketCompleteUpgrade(request, 0);

    WinHttpCloseHandle(request);

    DWORD timeout = 5000;
    WinHttpSetOption(
        ws,
        WINHTTP_OPTION_RECEIVE_TIMEOUT,
        &timeout,
        sizeof(timeout)
    );
    return ws;
}

void ws_send(HINTERNET ws, const char *c) {
    WinHttpWebSocketSend(ws,
        WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
        (PVOID)c,
        (DWORD)strlen(c));
}

char* ws_recv(HINTERNET ws) {
    DWORD size = 0;
    DWORD type = 0;
    char buffer[8192];

    DWORD result = WinHttpWebSocketReceive(
        ws,
        buffer,
        sizeof(buffer) - 1,
        &size,
        &type);

    if (result == ERROR_WINHTTP_TIMEOUT) {
        return NULL;
    }

    if (result != NO_ERROR) {
        printf("WebSocket receive failed: %lu\n", result);
        return NULL;
    }

    if (type == WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE) {
        return NULL;
    }

    buffer[size] = 0;
    return _strdup(buffer);
}

void close_websocket(HINTERNET ws) {
    WinHttpWebSocketClose(ws,
        WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS,
        NULL, 0);

    WinHttpCloseHandle(ws);
}
