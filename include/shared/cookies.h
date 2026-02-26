#ifndef DLL_CHROMIUM_COOKIE_H
#define DLL_CHROMIUM_COOKIE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct cookie_t {
    char *name;
    char *value;
    char *domain;
    char *path;
    char *expires;
    char *size;
    char *http_only;
    char *secure;
    char *session;
    char *same_site;
    char *priority;
    char *same_party;
    char *source_scheme;
    char *sourcePort;
} cookie_t;

#include <stdio.h>

void print_cookie(const cookie_t *cookie);
bool write_cookie_csv(FILE *file, const cookie_t *cookie);
void free_cookies(cookie_t *cookies, size_t count);
cookie_t* read_cookies_csv(FILE *file, size_t *count);

#endif
