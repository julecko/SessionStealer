#include <windows.h>
#include <string.h>
#include <stdio.h>

#define CMDLINE_MAX 1024

static int run_command(const char *cmdline, DWORD wait_seconds) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    HANDLE hNull = CreateFileA(
        "NUL",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    si.hStdInput  = hNull;
    si.hStdOutput = hNull;
    si.hStdError  = hNull;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, (LPSTR)cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        DWORD err = GetLastError();
        fprintf(stderr, "Failed to run '%s', error %lu\n", cmdline, err);
        return -1;
    }

    int exit_code = -1;

    if (wait_seconds == 0) {
        exit_code = 0;
    } else {
        DWORD wait_ms = wait_seconds * 1000;
        DWORD wait_result = WaitForSingleObject(pi.hProcess, wait_ms);

        if (wait_result == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            fprintf(stderr, "Process timed out and was terminated\n");
            exit_code = -1;
        } else if (wait_result == WAIT_FAILED) {
            DWORD err = GetLastError();
            fprintf(stderr, "WaitForSingleObject failed: %lu\n", err);
            exit_code = -1;
        } else {
            DWORD code;
            if (GetExitCodeProcess(pi.hProcess, &code)) {
                exit_code = (int)code;
            }
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exit_code;
}

int run_program(DWORD wait_seconds, const char *fmt, ...) {
    char cmdline[CMDLINE_MAX];
    va_list args;
    va_start(args, fmt);
    vsnprintf(cmdline, sizeof(cmdline), fmt, args);
    va_end(args);

    printf("Running program: %s\n", cmdline);
    return run_command(cmdline, wait_seconds);
}
