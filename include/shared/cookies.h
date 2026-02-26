#ifndef DLL_CHROMIUM_COOKIE_H
#define DLL_CHROMIUM_COOKIE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct cookie_t {
    const char *name;
    const char *value;
    const char *domain;
    const char *path;
    double expires;
    const char *size;
    bool http_only;
    bool secure;
    bool session;
    const char *same_site;
    const char *priority;
    bool same_party;
    const char *source_scheme;
    unsigned int sourcePort;
} cookie_t;


void print_cookie(const cookie_t *cookie);
bool write_cookie_csv(FILE *file, const cookie_t *cookie);
cookie_t* read_cookies_csv(const char *filename, size_t *count);

#endif
