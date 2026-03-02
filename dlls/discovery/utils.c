#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool file_exists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return false;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return false;
    return true;
}

bool query_reg_path(const char *reg_path, char *outbuffer, DWORD outbuffer_size) {
    HKEY hKey;

    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        reg_path,
        0,
        KEY_READ,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        return false;
    }

    DWORD size = outbuffer_size;
    result = RegQueryValueExA(
        hKey,
        NULL,
        NULL,
        NULL,
        (LPBYTE)outbuffer,
        &size
    );

    RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

/*const char *get_home_folder() {
    static char buffer[MAX_PATH];
    DWORD size = sizeof(buffer);

    DWORD ret = GetEnvironmentVariableA("USERPROFILE", buffer, size);
    if (ret > 0 && ret < size) {
        return buffer;
    }

    char username[256];
    DWORD username_len = sizeof(username);
    if (GetUserNameA(username, &username_len)) {
        snprintf(buffer, size, "C:\\Users\\%s", username);
        return buffer;
    }

    return NULL;
}
*/