#include "shared/cookies.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 2048

void free_cookies(cookie_t *cookies, size_t count) {
    if (!cookies) return;

    for (size_t i = 0; i < count; i++) {
        free((void*)cookies[i].name);
        free((void*)cookies[i].value);
        free((void*)cookies[i].domain);
        free((void*)cookies[i].path);
        free((void*)cookies[i].expires);
        free((void*)cookies[i].size);
        free((void*)cookies[i].http_only);
        free((void*)cookies[i].secure);
        free((void*)cookies[i].session);
        free((void*)cookies[i].same_site);
        free((void*)cookies[i].priority);
        free((void*)cookies[i].same_party);
        free((void*)cookies[i].source_scheme);
        free((void*)cookies[i].sourcePort);
    }

    free(cookies);
}

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
cookie_t *read_cookies_csv(FILE *file, size_t *count) {
    if (!file || !count) return NULL;

    size_t capacity = 1024;
    size_t n = 0;
    cookie_t *cookies = malloc(sizeof(cookie_t) * capacity);
    if (!cookies) {
        return NULL;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file)) {
        if (!strchr(line, '\n') && !feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            printf("Line too long, skipping\n");
            line[0] = '\0';
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }

        if (n >= capacity) {
            capacity *= 2;
            cookie_t *tmp = realloc(cookies, sizeof(cookie_t) * capacity);
            if (!tmp) {
                free_cookies(cookies, n);
                return NULL;
            }
            cookies = tmp;
        }

        char *fields[14] = {0};
        char *p = line;

        for (int i = 0; i < 14; i++) {
            if (!p) {
                fields[i] = NULL;
                continue;
            }
            fields[i] = p;

            char *q = strchr(p, ',');
            if (q) {
                *q = '\0';
                p = q + 1;
            } else {
                p = NULL;
            }
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

    *count = n;
    return cookies;
}

