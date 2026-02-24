#include <windows.h>
#include <stdio.h>
#include <string.h>

int file_exists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return 0;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return 0;
    return 1;
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