#include "dlls/firefox/load_cookies.h"
#include "shared/cookies.h"
#include "shared/cookie_utils.h"

#include <sqlite3.h>
#include <stdio.h>


void load_cookies(const char *cookie_file, const FILE *infile) {
    if (!infile) {
        fprintf(stderr, "Input file required\n");
        return;
    }

    size_t count = 0;
    cookie_t *cookies = read_cookies_csv(infile, &count);
    if (!cookies || count == 0) {
        printf("No cookies loaded\n");
        return;
    }

    sqlite3 *db;
    if (sqlite3_open(cookie_file, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql =
        "INSERT INTO moz_cookies ("
        "originAttributes, name, value, host, path, expiry, lastAccessed, "
        "creationTime, isSecure, isHttpOnly, inBrowserElement, sameSite, schemeMap"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?) "
        "ON CONFLICT(originAttributes, name, host, path) DO UPDATE SET "
        "value=excluded.value, "
        "expiry=excluded.expiry, "
        "lastAccessed=excluded.lastAccessed, "
        "creationTime=excluded.creationTime, "
        "isSecure=excluded.isSecure, "
        "isHttpOnly=excluded.isHttpOnly, "
        "sameSite=excluded.sameSite, "
        "schemeMap=excluded.schemeMap;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        free_cookies(cookies, count);
        return;
    }

    for (size_t i = 0; i < count; i++) {
        cookie_t *c = &cookies[i];

        sqlite3_bind_text(stmt, 1, c->browser.firefox.originAttributes ? c->browser.firefox.originAttributes : "", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, c->name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, c->value, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, c->domain, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, c->path, -1, SQLITE_STATIC);

        sqlite3_bind_int64(stmt, 6, expires_chromium_to_firefox(c->expires));

        sqlite3_bind_int64(stmt, 7, c->browser.firefox.last_accessed);
        sqlite3_bind_int64(stmt, 8, c->browser.firefox.creation_time);

        sqlite3_bind_int(stmt, 9, c->secure ? 1 : 0);
        sqlite3_bind_int(stmt, 10, c->http_only ? 1 : 0);

        sqlite3_bind_int(stmt, 11, (int)c->same_site);

        int source_scheme = 0;

        if (c->source_scheme == SCHEME_HTTPS)
            source_scheme |= 1;
        else if (c->source_scheme == SCHEME_HTTP)
            source_scheme |= 2;

        sqlite3_bind_int(stmt, 12, source_scheme);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    printf("Cookies inserted into Firefox\n");

    free_cookies(cookies, count);
}
