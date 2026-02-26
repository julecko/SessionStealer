#include "dlls/chromium/websocket.h"

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>

void fetch_cookies(const char *ws_url) {
    printf("[DEBUG] Connecting to WebSocket: %s\n", ws_url);

    HINTERNET ws = connect_websocket(ws_url);
    if (!ws) {
        printf("[ERROR] WebSocket connection failed\n");
        return;
    }

    printf("[DEBUG] WebSocket connected\n");

    int msg_id = 1;
    char msg[256];
    sprintf(msg,
        "{\"id\":%d,\"method\":\"Network.getAllCookies\"}",
        msg_id);

    printf("[DEBUG] Sending Network.getAllCookies (id=%d)\n", msg_id);
    ws_send(ws, msg);

    printf("[DEBUG] Waiting for cookies response...\n");

    while (1) {
        char *resp = ws_recv(ws);
        if (!resp) {
            printf("[ERROR] ws_recv returned NULL\n");
            break;
        }

        printf("[DEBUG] Received message:\n%s\n", resp);

        /*cJSON *json = cJSON_Parse(resp);
        free(resp);

        if (!json) {
            printf("[WARN] Failed to parse JSON message\n");
            continue;
        }

        //cJSON *id = cJSON_GetObjectItem(json, "id");

        // Ignore unrelated CDP events
        if (!id) {
            printf("[DEBUG] Ignoring event (no id field)\n");
            cJSON_Delete(json);
            continue;
        }

        printf("[DEBUG] Message id=%d\n", id->valueint);

        if (id->valueint == msg_id) {
            printf("[DEBUG] Matched cookie response\n");

            cJSON *result = cJSON_GetObjectItem(json, "result");
            if (!result) {
                printf("[ERROR] No 'result' field in response\n");
                cJSON_Delete(json);
                break;
            }

            cJSON *cookies = cJSON_GetObjectItem(result, "cookies");
            if (!cookies || !cJSON_IsArray(cookies)) {
                printf("[ERROR] No 'cookies' array found\n");
                cJSON_Delete(json);
                break;
            }

            int count = cJSON_GetArraySize(cookies);
            printf("[DEBUG] Cookie count: %d\n", count);

            for (int i = 0; i < count; i++) {
                cJSON *c = cJSON_GetArrayItem(cookies, i);
                if (!c) continue;

                cJSON *domain = cJSON_GetObjectItem(c, "domain");
                cJSON *name   = cJSON_GetObjectItem(c, "name");
                cJSON *value  = cJSON_GetObjectItem(c, "value");

                if (domain && name && value &&
                    cJSON_IsString(domain) &&
                    cJSON_IsString(name) &&
                    cJSON_IsString(value)) {

                    printf("%s | %s = %s\n",
                        domain->valuestring,
                        name->valuestring,
                        value->valuestring);
                }
            }

            cJSON_Delete(json);
            break;
        }

        cJSON_Delete(json);*/
    }

    printf("[DEBUG] Closing WebSocket\n");

    close_websocket(ws);

    printf("[DEBUG] WebSocket closed\n");
}
