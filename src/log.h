#ifndef C_WEATHER_LOG_H
#define C_WEATHER_LOG_H

#include <stdarg.h>

/* UTF-8 log to stderr (Windows console uses WriteConsoleW). */
void log_errf(const char *fmt, ...);

#endif /* C_WEATHER_LOG_H */
