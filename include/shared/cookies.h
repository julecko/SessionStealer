#ifndef DLL_CHROMIUM_COOKIE_H
#define DLL_CHROMIUM_COOKIE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct cookie_t {
    const char *name;
    const char *value;
    const char *domain;
    const char *path;
    const char *expires;
    const char *size;
    const char *http_only;
    const char *secure;
    const char *session;
    const char *same_site;
    const char *priority;
    const char *same_party;
    const char *source_scheme;
    const char *sourcePort;
} cookie_t;


void print_cookie(const cookie_t *cookie);
bool write_cookie_csv(FILE *file, const cookie_t *cookie);
cookie_t* read_cookies_csv(const char *filename, size_t *count);

#endif
