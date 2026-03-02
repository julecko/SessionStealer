#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

int get_user_data_dir(char *user_data_dir);
int run_program(DWORD wait_seconds, const char *fmt, ...);

#endif
