#include "dlls/firefox/fetch_cookies.h"
#include "shared/cookies.h"
#include "shared/cookie_utils.h"
#include <stdio.h>
#include <sqlite3.h>

static void process_cookie_row(sqlite3_stmt *stmt, cookie_t *c) {
    const unsigned char *name  = sqlite3_column_text(stmt, 2);
    const unsigned char *value = sqlite3_column_text(stmt, 3);
    const unsigned char *host  = sqlite3_column_text(stmt, 4);
    const unsigned char *path  = sqlite3_column_text(stmt, 5);
    sqlite3_int64 expiry        = sqlite3_column_int64(stmt, 6);
    int isSecure                = sqlite3_column_int(stmt, 8);
    int isHttpOnly              = sqlite3_column_int(stmt, 9);
    int sameSite                = sqlite3_column_int(stmt, 11);
    sqlite3_int64 creationTime  = sqlite3_column_int64(stmt, 14);
    sqlite3_int64 lastAccessed  = sqlite3_column_int64(stmt, 7);
    const unsigned char *originAttributes = sqlite3_column_text(stmt, 1);
    int schemeMap = sqlite3_column_int(stmt, 12);

    c->name   = name  ? (char*)name  : "";
    c->value  = value ? (char*)value : "";
    c->domain = host  ? (char*)host  : "";
    c->path   = path  ? (char*)path  : "";
    c->expires = expiry;
    c->secure = isSecure ? true : false;
    c->http_only = isHttpOnly ? true : false;
    c->session = (expiry == 0);
    c->same_site = (sameSite >= 0 && sameSite <= 2) ? (cookie_samesite_t)sameSite : COOKIE_SAMESITE_NONE;

    c->browser.firefox.creation_time = creationTime;
    c->browser.firefox.last_accessed = lastAccessed;
    c->browser.firefox.originAttributes = originAttributes ? (char*)originAttributes : "";

    if (schemeMap & 1) {
        c->source_scheme = SCHEME_HTTPS;
    } else {
        c->source_scheme = SCHEME_HTTP;
    }
}

void fetch_cookies(const char *cookie_file, FILE *outfile) {
    if (!cookie_file || !outfile) {
        fprintf(stderr, "Invalid arguments for function fetch cookies\n");
        return;
    }

    if (!init_cookie_csv(outfile)) {
        fputs("Failed to write csv file header\n", stderr);
        return;
    }

    sqlite3 *db;
    if (sqlite3_open(cookie_file, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "SELECT "
    "id, originAttributes, name, value, host, path, expiry, lastAccessed, "
    "isSecure, isHttpOnly, inBrowserElement, sameSite, schemeMap, "
    "isPartitionedAttributeSet, creationTime, updateTime "
    "FROM moz_cookies;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    int col_count = sqlite3_column_count(stmt);

    if (col_count < 16) {
        fputs("Unexpected schema!\n", stderr);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cookie_t cookie = {0};
        cookie.browser_type = BROWSER_FIREFOX;

        process_cookie_row(stmt, &cookie);
        cookie.expires = expires_firefox_to_chromium(cookie.expires);
        write_cookie_csv(outfile, &cookie);
    }

    printf("Cookies extracted from firefox\n");

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return;
}
