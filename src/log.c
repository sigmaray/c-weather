#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

void log_errf(const char *fmt, ...) {
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

#if defined(_WIN32) && !defined(__CYGWIN__)
    HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && h != NULL && GetConsoleMode(h, &mode)) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
        if (wlen > 0) {
            wchar_t *wbuf = (wchar_t *)malloc((size_t)wlen * sizeof(wchar_t));
            if (wbuf) {
                if (MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, wlen) > 0) {
                    DWORD written = 0;
                    WriteConsoleW(h, wbuf, (DWORD)(wlen - 1), &written, NULL);
                }
                free(wbuf);
                return;
            }
        }
    }
#endif
    fputs(buf, stderr);
    fflush(stderr);
}
