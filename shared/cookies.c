#include "shared/cookies.h"

#include <stdio.h>

#define MAX_LINE 1024

void print_cookie(const cookie_t *c) {
    if (!c) return;

    printf(
        "name=%s value=%s domain=%s path=%s expires=%s size=%s "
        "http_only=%s secure=%s session=%s same_site=%s priority=%s same_party=%s "
        "source_scheme=%s sourcePort=%s\n",
        c->name ? c->name : "",
        c->value ? c->value : "",
        c->domain ? c->domain : "",
        c->path ? c->path : "",
        c->expires ? c->expires : "",
        c->size ? c->size : "",
        c->http_only ? c->http_only : "",
        c->secure ? c->secure : "",
        c->session ? c->session : "",
        c->same_site ? c->same_site : "",
        c->priority ? c->priority : "",
        c->same_party ? c->same_party : "",
        c->source_scheme ? c->source_scheme : "",
        c->sourcePort ? c->sourcePort : ""
    );
}

bool write_cookie_csv(FILE *file, const cookie_t *c) {
    if (!file || !c) return false;

    return fprintf(file,
        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
        c->name ? c->name : "",
        c->value ? c->value : "",
        c->domain ? c->domain : "",
        c->path ? c->path : "",
        c->expires ? c->expires : "",
        c->size ? c->size : "",
        c->http_only ? c->http_only : "",
        c->secure ? c->secure : "",
        c->session ? c->session : "",
        c->same_site ? c->same_site : "",
        c->priority ? c->priority : "",
        c->same_party ? c->same_party : "",
        c->source_scheme ? c->source_scheme : "",
        c->sourcePort ? c->sourcePort : ""
    ) > 0;
}

// Must free
cookie_t *read_cookies_csv(const char *filename, size_t *count) {
    if (!filename || !count) return NULL;

    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    size_t capacity = 16;
    size_t n = 0;
    cookie_t *cookies = malloc(sizeof(cookie_t) * capacity);
    if (!cookies) {
        fclose(f);
        return NULL;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        if (n >= capacity) {
            capacity *= 2;
            cookie_t *tmp = realloc(cookies, sizeof(cookie_t) * capacity);
            if (!tmp) {
                free_cookies(cookies, n);
                fclose(f);
                return NULL;
            }
            cookies = tmp;
        }

        char *fields[14] = {0};
        char *p = line;

        for (int i = 0; i < 14; i++) {
            fields[i] = p;
            char *q = strchr(p, ',');
            if (!q) break;
            *q = '\0';
            p = q + 1;
        }

        cookies[n].name          = fields[0]  ? strdup(fields[0])  : strdup("");
        cookies[n].value         = fields[1]  ? strdup(fields[1])  : strdup("");
        cookies[n].domain        = fields[2]  ? strdup(fields[2])  : strdup("");
        cookies[n].path          = fields[3]  ? strdup(fields[3])  : strdup("");
        cookies[n].expires       = fields[4]  ? strdup(fields[4])  : strdup("");
        cookies[n].size          = fields[5]  ? strdup(fields[5])  : strdup("");
        cookies[n].http_only     = fields[6]  ? strdup(fields[6])  : strdup("");
        cookies[n].secure        = fields[7]  ? strdup(fields[7])  : strdup("");
        cookies[n].session       = fields[8]  ? strdup(fields[8])  : strdup("");
        cookies[n].same_site     = fields[9]  ? strdup(fields[9])  : strdup("");
        cookies[n].priority      = fields[10] ? strdup(fields[10]) : strdup("");
        cookies[n].same_party    = fields[11] ? strdup(fields[11]) : strdup("");
        cookies[n].source_scheme = fields[12] ? strdup(fields[12]) : strdup("");
        cookies[n].sourcePort    = fields[13] ? strdup(fields[13]) : strdup("");

        n++;
    }

    fclose(f);
    *count = n;
    return cookies;
}

