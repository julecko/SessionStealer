#include "shared/cookies.h"

#include <stdio.h>

#define MAX_LINE 1024

void print_cookie(const cookie_t *c) {
    if (!c) return;
    printf(
        "name=%s value=%s domain=%s path=%s expires=%.0f size=%s "
        "http_only=%d secure=%d session=%d same_site=%s priority=%s same_party=%d "
        "source_scheme=%s sourcePort=%u\n",
        c->name ? c->name : "",
        c->value ? c->value : "",
        c->domain ? c->domain : "",
        c->path ? c->path : "",
        c->expires,
        c->size ? c->size : "",
        c->http_only,
        c->secure,
        c->session,
        c->same_site ? c->same_site : "",
        c->priority ? c->priority : "",
        c->same_party,
        c->source_scheme ? c->source_scheme : "",
        c->sourcePort
    );
}

bool write_cookie_csv(FILE *file, const cookie_t *c) {
    if (!file || !c) return false;

    return fprintf(file,
        "%s,%s,%s,%s,%.0f,%s,%d,%d,%d,%s,%s,%d,%s,%u\n",
        c->name ? c->name : "",
        c->value ? c->value : "",
        c->domain ? c->domain : "",
        c->path ? c->path : "",
        c->expires,
        c->size ? c->size : "",
        c->http_only,
        c->secure,
        c->session,
        c->same_site ? c->same_site : "",
        c->priority ? c->priority : "",
        c->same_party,
        c->source_scheme ? c->source_scheme : "",
        c->sourcePort) > 0;
}

// Must free
cookie_t *read_cookies_csv(const char *filename, size_t *count) {
    if (!filename || !count) return NULL;
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    size_t capacity = 16;
    size_t n = 0;
    cookie_t *cookies = malloc(sizeof(cookie_t) * capacity);
    if (!cookies) { fclose(f); return NULL; }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        if (n >= capacity) {
            capacity *= 2;
            cookie_t *tmp = realloc(cookies, sizeof(cookie_t) * capacity);
            if (!tmp) { free_cookies(cookies, n); fclose(f); return NULL; }
            cookies = tmp;
        }

        // Split line by ','
        char *fields[14] = {0};
        char *p = line;
        for (int i = 0; i < 14; i++) {
            fields[i] = p;
            char *q = strchr(p, ',');
            if (!q) break;
            *q = '\0';
            p = q + 1;
        }

        cookies[n].name = strdup(fields[0]);
        cookies[n].value = strdup(fields[1]);
        cookies[n].domain = strdup(fields[2]);
        cookies[n].path = strdup(fields[3]);
        cookies[n].expires = fields[4] ? atof(fields[4]) : 0;
        cookies[n].size = strdup(fields[5]);
        cookies[n].http_only = fields[6] ? atoi(fields[6]) != 0 : false;
        cookies[n].secure = fields[7] ? atoi(fields[7]) != 0 : false;
        cookies[n].session = fields[8] ? atoi(fields[8]) != 0 : false;
        cookies[n].same_site = strdup(fields[9]);
        cookies[n].priority = strdup(fields[10]);
        cookies[n].same_party = fields[11] ? atoi(fields[11]) != 0 : false;
        cookies[n].source_scheme = strdup(fields[12]);
        cookies[n].sourcePort = fields[13] ? (unsigned int)atoi(fields[13]) : 0;

        n++;
    }

    fclose(f);
    *count = n;
    return cookies;
}
