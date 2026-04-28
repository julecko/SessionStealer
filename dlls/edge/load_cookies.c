#include "dlls/edge/load_cookies.h"
#include "dlls/edge/edge.h"
#include "shared/cookies.h"
#include "shared/cookie_utils.h"

#include <windows.h>
#include <winhttp.h>
#include <inttypes.h>
#include <stdbool.h>

// Caller must free
static char *json_escape(const char *input) {
    if (!input) return _strdup("");

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
                    p += sprintf_s(p, sizeof(p), "\\u%04x", c);
                } else {
                    *p++ = c;
                }
                break;
        }
    }
    *p = '\0';
    return out;
}

static char *generate_cookie_json(const cookie_t *c, int message_id) {
    if (!c || !c->name || !c->domain || !c->path) return NULL;

    char *name   = json_escape(c->name);
    char *value  = json_escape(c->value ? c->value : "");
    char *domain = json_escape(c->domain);
    char *path   = json_escape(c->path);

    const char *same_site_str = "Lax";
    switch (c->same_site) {
        case COOKIE_SAMESITE_NONE:   same_site_str = "None"; break;
        case COOKIE_SAMESITE_LAX:    same_site_str = "Lax"; break;
        case COOKIE_SAMESITE_STRICT: same_site_str = "Strict"; break;
    }

    bool secure = c->secure;
    if (c->same_site == COOKIE_SAMESITE_NONE) {
        secure = true;
    }

    char *json = malloc(4096);
    if (!json) {
        free(name); free(value); free(domain); free(path);
        return NULL;
    }

    snprintf(json, 4096,
        "{"
        "\"id\":%d,"
        "\"method\":\"Network.setCookie\","
        "\"params\":{"
            "\"name\":\"%s\","
            "\"value\":\"%s\","
            "\"domain\":\"%s\","
            "\"path\":\"%s\","
            "\"secure\":%s,"
            "\"httpOnly\":%s,"
            "\"sameSite\":\"%s\","
            "\"expires\":%" PRId64
        "}"
        "}",
        message_id,
        name, value, domain, path,
        secure ? "true" : "false",
        c->http_only ? "true" : "false",
        same_site_str,
        c->expires > 0 ? c->expires : 0
    );

    free(name); free(value); free(domain); free(path);
    return json;
}

void load_cookies(const char *ws_url, FILE *infile, bool json_only) {
    if (!infile) {
        fprintf(stderr, "Input file required\n");
        return;
    }

    size_t count = 0;
    cookie_t *cookies = read_cookies_csv(infile, &count);
    if (!cookies || count == 0) {
        printf("No cookies loaded\n");
        return;
    }

    int message_id = 1;
    FILE *json_file = NULL;
    HINTERNET ws = NULL;

    if (json_only) {
        if (fopen_s(&json_file, "cookies.json", "w") != 0) {
            fprintf(stderr, "Failed to open cookies.json\n");
            free_cookies(cookies, count);
            return;
        }

        fprintf(json_file, "[\n");
    } else {
        if (!ws_url) {
            fprintf(stderr, "WebSocket URL required\n");
            free_cookies(cookies, count);
            return;
        }

        ws = connect_websocket_ptr ? connect_websocket_ptr(ws_url) : NULL;
        if (!ws) {
            fprintf(stderr, "WebSocket connection failed\n");
            free_cookies(cookies, count);
            return;
        }

        char enable_msg[128];
        snprintf(enable_msg, sizeof(enable_msg),
                 "{\"id\":%d,\"method\":\"Network.enable\"}", message_id++);
        if (ws_send_ptr) ws_send_ptr(ws, enable_msg);
    }

    for (size_t i = 0; i < count; i++) {
        char *cookie_json = generate_cookie_json(&cookies[i], message_id++);
        if (!cookie_json) continue;

        if (json_only) {
            fprintf(json_file, "%s%s\n",
                    cookie_json,
                    (i + 1 < count) ? "," : "");
        } else {
            if (ws_send_ptr) ws_send_ptr(ws, cookie_json);

            char *resp = NULL;
            bool finished = false;
            if (ws_recv_ptr && ws_recv_ptr(ws, &resp, &finished)) {
                printf("Cookie '%s' response: %s\n", cookies[i].name, resp);
                free(resp);
            } else {
                printf("No response for cookie '%s'\n", cookies[i].name);
            }
        }

        free(cookie_json);
    }

    if (json_only) {
        fprintf(json_file, "]\n");
        fclose(json_file);
        printf("Exported %zu cookies to cookies.json\n", count);
    } else {
        char reload_msg[128];
        snprintf(reload_msg, sizeof(reload_msg),
                 "{\"id\":%d,\"method\":\"Page.reload\"}", message_id++);
        if (ws_send_ptr) ws_send_ptr(ws, reload_msg);

        char *resp = NULL; bool finished = false;
        if (ws_recv_ptr && ws_recv_ptr(ws, &resp, &finished)) free(resp);

        if (close_websocket_ptr) close_websocket_ptr(ws);
        printf("Loaded %zu cookies via WebSocket\n", count);
    }

    free_cookies(cookies, count);
}
