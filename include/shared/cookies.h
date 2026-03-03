#ifndef DLL_SHARED_COOKIE_H
#define DLL_SHARED_COOKIE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    BROWSER_CHROMIUM = 0,
    BROWSER_FIREFOX  = 1
} browser_type_t;

typedef enum {
    COOKIE_SAMESITE_NONE = 0,
    COOKIE_SAMESITE_LAX = 1,
    COOKIE_SAMESITE_STRICT = 2
} cookie_samesite_t;

typedef enum {
    COOKIE_PRIORITY_LOW = 0,
    COOKIE_PRIORITY_MEDIUM = 1,
    COOKIE_PRIORITY_HIGH = 2
} cookie_priority_t;

typedef enum {
    SCHEME_UNSET = 0,
    SCHEME_HTTP  = 1,
    SCHEME_HTTPS = 2
} source_scheme_t;

typedef struct {
    cookie_priority_t priority;
    bool same_party;
    int source_port;
} chromium_fields_t;

typedef struct {
    int64_t creation_time;    // microseconds since epoch
    int64_t last_accessed;    // microseconds since epoch
    char *originAttributes;   // container / first-party isolation
} firefox_fields_t;

typedef struct cookie_t {
    char *name;
    char *value;
    char *domain;
    char *path;

    uint64_t expires; // Chromium default, microseconds since 1601
    bool http_only;
    bool secure;
    bool session;
    cookie_samesite_t same_site;
    source_scheme_t source_scheme;
    
    browser_type_t browser_type;

    union {
        chromium_fields_t chromium;
        firefox_fields_t firefox;
    } browser;
} cookie_t;

bool init_cookie_csv(FILE *file);
void print_cookie(const cookie_t *cookie);
bool write_cookie_csv(FILE *file, const cookie_t *cookie);
void free_cookies(cookie_t *cookies, size_t count);
cookie_t* read_cookies_csv(FILE *file, size_t *count);

#endif
