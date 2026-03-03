#include "dlls/chromium/load_cookies.h"
#include "dlls/chromium/websocket.h"
#include "shared/cookies.h"
#include "shared/cookie_utils.h"

#include <windows.h>
#include <winhttp.h>
#include <inttypes.h>

// Caller must free
static char *json_escape(const char *input) {
    if (!input) return strdup("");

    size_t len = strlen(input);
    size_t max_len = len * 6 + 1;
    char *out = malloc(max_len);
    if (!out) return NULL;

    char *p = out;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = input[i];
        switch (c) {
            case '"':  *p++ = '\\'; *p++ = '"';  break;
            case '\\': *p++ = '\\'; *p++ = '\\'; break;
            case '\b': *p++ = '\\'; *p++ = 'b';  break;
            case '\f': *p++ = '\\'; *p++ = 'f';  break;
            case '\n': *p++ = '\\'; *p++ = 'n';  break;
            case '\r': *p++ = '\\'; *p++ = 'r';  break;
            case '\t': *p++ = '\\'; *p++ = 't';  break;
            default:
                if (c <= 0x1F) {
                    p += sprintf(p, "\\u%04x", c);
                } else {
                    *p++ = c;
                }
                break;
        }
    }
    *p = '\0';
    return out;
}

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
        const cookie_t *c = &cookies[i];

        if (!c->name || !c->value || !c->domain || !c->path)
            continue;

        if (written > 0)
            offset += snprintf(json + offset, buffer_size - offset, ",");

        char *name = json_escape(c->name);
        char *value = json_escape(c->value);
        char *domain = json_escape(c->domain);
        char *path = json_escape(c->path);

        const char *same_site_str = "Lax";
        switch (c->same_site) {
            case COOKIE_SAMESITE_NONE:   same_site_str = "None"; break;
            case COOKIE_SAMESITE_LAX:    same_site_str = "Lax"; break;
            case COOKIE_SAMESITE_STRICT: same_site_str = "Strict"; break;
        }

        const char *priority_str = "Medium";
        switch (c->browser.chromium.priority) {
            case COOKIE_PRIORITY_LOW:    priority_str = "Low"; break;
            case COOKIE_PRIORITY_MEDIUM: priority_str = "Medium"; break;
            case COOKIE_PRIORITY_HIGH:   priority_str = "High"; break;
        }

        int64_t expires = 0;
        if (c->browser_type == BROWSER_FIREFOX) {
            expires = expires_firefox_to_chromium(c->expires);
        } else if (c->browser_type == BROWSER_CHROMIUM) {
            expires = c->expires;
        }

        offset += snprintf(json + offset, buffer_size - offset,
            "{"
            "\"name\":\"%s\","
            "\"value\":\"%s\","
            "\"domain\":\"%s\","
            "\"path\":\"%s\","
            "\"expires\":%" PRId64 ","
            "\"secure\":%s,"
            "\"httpOnly\":%s,"
            "\"sameSite\":\"%s\","
            "\"priority\":\"%s\""
            "}",
            name,
            value,
            domain,
            path,
            expires,
            c->secure ? "true" : "false",
            c->http_only ? "true" : "false",
            same_site_str,
            priority_str
        );

        free(name);
        free(value);
        free(domain);
        free(path);

        written++;
    }

    snprintf(json + offset, buffer_size - offset,
        "]}}"
    );

    FILE *temp = fopen("temp.json", "w");
    fprintf(temp, "%s", json);
    fclose(temp);

    ws_send(ws, json);

    ws_send(ws, "{\"id\":3,\"method\":\"Page.reload\"}");

    free(json);
    close_websocket(ws);
    free_cookies(cookies, count);

    printf("Cookies (%zu) loaded into browser\n", count);
}
