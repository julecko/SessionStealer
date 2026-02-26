#include "dlls/chromium/load_cookies.h"
#include "dlls/chromium/websocket.h"
#include "shared/cookies.h"

#include <windows.h>
#include <winhttp.h>

void load_cookies(const char *ws_url, const FILE *infile) {
    if (!ws_url || !infile) {
        printf("Load cookies: invalid arguments\n");
        return;
    }
    
    size_t count = 0;
    cookie_t *cookies = read_cookies_csv(infile, &count);
    if (!cookies || count == 0) {
        printf("No cookies loaded from file\n");
        return;
    }

    HINTERNET ws = connect_websocket(ws_url);
    if (!ws) {
        printf("WebSocket connection failed\n");
        free_cookies(cookies, count);
        return;
    }

    ws_send(ws, "{\"id\":1,\"method\":\"Network.enable\"}");

    // Large buffer (increase if you expect many cookies)
    size_t buffer_size = 1024 * 1024 * 10;
    char *json = malloc(buffer_size);
    if (!json) {
        printf("Malloc failed\n");
        close_websocket(ws);
        free_cookies(cookies, count);
        return;
    }

    size_t offset = 0;

    offset += snprintf(json + offset, buffer_size - offset,
        "{"
        "\"id\":2,"
        "\"method\":\"Network.setCookies\","
        "\"params\":{"
        "\"cookies\":["
    );

    size_t written = 0;

    for (size_t i = 0; i < count; i++) {
        cookie_t *c = &cookies[i];

        if (!c->name || !c->value || !c->domain || !c->path)
            continue;

        if (written > 0)
            offset += snprintf(json + offset, buffer_size - offset, ",");

        double expires = 0.0;
        if (c->expires && strlen(c->expires))
            expires = atof(c->expires);

        const char *secure =
            (c->secure && strcmp(c->secure, "true") == 0) ? "true" : "false";

        const char *http_only =
            (c->http_only && strcmp(c->http_only, "true") == 0) ? "true" : "false";

        offset += snprintf(json + offset, buffer_size - offset,
            "{"
            "\"name\":\"%s\","
            "\"value\":\"%s\","
            "\"domain\":\"%s\","
            "\"path\":\"%s\","
            "\"expires\":%f,"
            "\"secure\":%s,"
            "\"httpOnly\":%s,"
            "\"sameSite\":\"%s\""
            "}",
            c->name,
            c->value,
            c->domain,
            c->path,
            expires,
            secure,
            http_only,
            (c->same_site && strlen(c->same_site)) ? c->same_site : "Lax"
        );

        written++;
    }

    offset += snprintf(json + offset, buffer_size - offset,
        "]}}"
    );

    ws_send(ws, json);

    ws_send(ws, "{\"id\":3,\"method\":\"Page.reload\"}");

    free(json);
    close_websocket(ws);
    free_cookies(cookies, count);

    printf("Cookies loaded into browser\n");
}
