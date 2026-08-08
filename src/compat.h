#ifndef C_WEATHER_COMPAT_H
#define C_WEATHER_COMPAT_H

#include <string.h>
#include <time.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <string.h>
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
static inline struct tm *localtime_r(const time_t *timep, struct tm *result) {
    if (localtime_s(result, timep) != 0) {
        return NULL;
    }
    return result;
}
#else
#include <strings.h>
#endif

#endif /* C_WEATHER_COMPAT_H */
