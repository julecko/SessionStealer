#include "shared/cookies.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 2048
#define COOKIE_HEADER "browser_type,name,value,domain,path,expires,http_only,secure,session," \
                      "same_site,priority,same_party,source_scheme,sourcePort,creation_time,last_accessed,originAttributes"

bool init_cookie_csv(FILE *file) {
    if (!file) return false;
    return fprintf(file, "%s\n", COOKIE_HEADER) > 0;
}

void free_cookie(cookie_t *cookie) {
    free(cookie->name);
    free(cookie->value);
    free(cookie->domain);
    free(cookie->path);
    if (cookie->browser_type == BROWSER_FIREFOX) {
        free(cookie->browser.firefox.originAttributes);
    }
}

void free_cookies(cookie_t *cookies, size_t count) {
    if (!cookies) return;

    for (size_t i = 0; i < count; i++) {
        free_cookie(&cookies[i]);
    }
    free(cookies);
}

void print_cookie(const cookie_t *c) {
    if (!c) return;

    if (c->browser_type == BROWSER_CHROMIUM) {
        printf(
            "Chromium: name=%s value=%s domain=%s path=%s expires=%lld "
            "http_only=%d secure=%d session=%d same_site=%d priority=%d same_party=%d "
            "source_scheme=%d sourcePort=%d\n",
            c->name, c->value, c->domain, c->path, (int64_t)c->expires,
            c->http_only, c->secure, c->session, c->same_site,
            c->browser.chromium.priority,
            c->browser.chromium.same_party,
            c->source_scheme,
            c->browser.chromium.source_port
        );
    } else {
        printf(
            "Firefox: name=%s value=%s domain=%s path=%s expires=%lld "
            "http_only=%d secure=%d session=%d same_site=%d "
            "creation_time=%lld last_accessed=%lld originAttributes=%s\n",
            c->name, c->value, c->domain, c->path, c->expires,
            c->http_only, c->secure, c->session, c->same_site,
            (long long)c->browser.firefox.creation_time,
            (long long)c->browser.firefox.last_accessed,
            c->browser.firefox.originAttributes ? c->browser.firefox.originAttributes : ""
        );
    }
}

bool write_cookie_csv(FILE *file, const cookie_t *c) {
    if (!file || !c) return false;

    if (c->browser_type == BROWSER_CHROMIUM) {
        return fprintf(file,
            "%d,%s,%s,%s,%s,%lld,%d,%d,%d,%d,%d,%d,%d,%d,,,\n",
            c->browser_type,
            c->name ? c->name : "",
            c->value ? c->value : "",
            c->domain ? c->domain : "",
            c->path ? c->path : "",
            c->expires,
            c->http_only,
            c->secure,
            c->session,
            c->same_site,
            c->browser.chromium.priority,
            c->browser.chromium.same_party,
            c->source_scheme,
            c->browser.chromium.source_port
        ) > 0;
    } else if (c->browser_type == BROWSER_FIREFOX) { // Firefox
        return fprintf(file,
            "%d,%s,%s,%s,%s,%lld,%d,%d,%d,%d,,,,,%lld,%lld,%s\n",
            c->browser_type,
            c->name ? c->name : "",
            c->value ? c->value : "",
            c->domain ? c->domain : "",
            c->path ? c->path : "",
            c->expires,
            c->http_only,
            c->secure,
            c->session,
            c->same_site,
            (long long)c->browser.firefox.creation_time,
            (long long)c->browser.firefox.last_accessed,
            c->browser.firefox.originAttributes ? c->browser.firefox.originAttributes : ""
        ) > 0;
    }
}

cookie_t *read_cookies_csv(FILE *file, size_t *count) {
    if (!file || !count) return NULL;

    char line[MAX_LINE];

    if (!fgets(line, sizeof(line), file)) return NULL;
    line[strcspn(line, "\r\n")] = '\0';
    if (strcmp(line, COOKIE_HEADER) != 0) {
        printf("Invalid CSV header\n");
        return NULL;
    }

    size_t capacity = 1024;
    size_t n = 0;
    cookie_t *cookies = malloc(sizeof(cookie_t) * capacity);
    if (!cookies) return NULL;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        if (n >= capacity) {
            capacity *= 2;
            cookie_t *tmp = realloc(cookies, sizeof(cookie_t) * capacity);
            if (!tmp) { free_cookies(cookies, n); return NULL; }
            cookies = tmp;
        }

        char *fields[18] = {0};
        char *p = line;
        for (int i = 0; i < 18; i++) {
            if (!p) { fields[i] = NULL; continue; }
            fields[i] = p;
            char *q = strchr(p, ',');
            if (q) { *q = '\0'; p = q + 1; } else { p = NULL; }
        }

        cookie_t *c = &cookies[n];
        memset(c, 0, sizeof(cookie_t));

        c->browser_type = fields[0] ? atoi(fields[0]) : BROWSER_CHROMIUM;

        c->name   = fields[1] ? strdup(fields[1]) : strdup("");
        c->value  = fields[2] ? strdup(fields[2]) : strdup("");
        c->domain = fields[3] ? strdup(fields[3]) : strdup("");
        c->path   = fields[4] ? strdup(fields[4]) : strdup("");
        c->expires = fields[5] ? atoll(fields[5]) : 0;
        c->http_only = fields[6] ? atoi(fields[6]) : 0;
        c->secure    = fields[7] ? atoi(fields[7]) : 0;
        c->session   = fields[8] ? atoi(fields[8]) : 0;
        c->same_site = fields[9] ? atoi(fields[9]) : COOKIE_SAMESITE_NONE;

        if (c->browser_type == BROWSER_CHROMIUM) {
            c->browser.chromium.priority    = fields[10] ? atoi(fields[10]) : COOKIE_PRIORITY_LOW;
            c->browser.chromium.same_party  = fields[11] ? atoi(fields[11]) : 0;
            c->source_scheme                = fields[12] ? atoi(fields[12]) : SCHEME_UNSET;
            c->browser.chromium.source_port = fields[13] ? atoi(fields[13]) : 0;
        } else if (c->browser_type == BROWSER_FIREFOX) {
            c->browser.firefox.creation_time    = fields[14] ? atoll(fields[14]) : 0;
            c->browser.firefox.last_accessed    = fields[15] ? atoll(fields[15]) : 0;
            c->browser.firefox.originAttributes = fields[16] ? strdup(fields[16]) : strdup("");
        }

        n++;
    }

    *count = n;
    return cookies;
}
