#include "dlls/firefox/utils.h"
#include "shared/util.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

void normalize_ini_path(char *path) {
    if (!path)
        return;

    for (size_t i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/')
            path[i] = '\\';
    }
}

int get_cookie_file(char *out, size_t out_size) {
    if (!out || out_size == 0)
        return -1;

    char appdata[MAX_PATH];
    DWORD len = GetEnvironmentVariableA("APPDATA", appdata, MAX_PATH);
    if (len == 0 || len >= MAX_PATH)
        return -1;

    char ini_path[MAX_PATH];
    snprintf(ini_path, sizeof(ini_path),
             "%s\\Mozilla\\Firefox\\profiles.ini", appdata);

    FILE *f;
    if (fopen_s(&f, ini_path, "r") != 0)
        return -1;

    char line[512];
    char profile_path[MAX_PATH] = {0};

    char tmp_path[MAX_PATH] = {0};
    int tmp_default = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;

        if (strncmp(line, "[Profile", 8) == 0) {
            tmp_default = 0;
            tmp_path[0] = 0;
            continue;
        }

        if (strncmp(line, "Name=default-release", 20) == 0) {
            tmp_default = 1;
        }

        if (strncmp(line, "Path=", 5) == 0) {
            strncpy_s(tmp_path, sizeof(tmp_path), line + 5, sizeof(tmp_path) - 1);
        }

        if (tmp_default && tmp_path[0] != 0) {
            strncpy_s(profile_path, sizeof(tmp_path), tmp_path, sizeof(profile_path) - 1);
            break;
        }
    }

    fclose(f);

    if (profile_path[0] == 0)
        return -1;

    normalize_ini_path(profile_path);

    snprintf(out, out_size,
             "%s\\Mozilla\\Firefox\\%s\\cookies.sqlite",
             appdata,
             profile_path);

    return (int)strlen(out);
}
