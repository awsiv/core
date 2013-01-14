/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "log.h"
#include "string_lib.h"

#include <stdarg.h>
#include <syslog.h>

LogPerformanceTimer LogPerformanceStart(void)
{
    struct timeval start = { 0 };
    struct timezone tz = { 0 };

    if (gettimeofday(&start, &tz) != 0)
    {
        syslog(LOG_DEBUG, "Unable to start timer");
        return start;
    }
    return start;
}

void LogPerformanceStop(LogPerformanceTimer *timer, const char *format, ...)
{
    if (timerisset(timer))
    {
        struct timeval stop = { 0 };
        struct timezone tz = { 0 };

        if (gettimeofday(&stop, &tz) != 0)
        {
            syslog(LOG_DEBUG, "Unable to capture stop timer");
            return;
        }

        struct timeval duration = { 0 };
        timersub(&stop, timer, &duration);

        char buffer[1024] = { 0 };
        snprintf(buffer, 1024, "[perf] %d sec, %d ms: ", (int)duration.tv_sec, (int)(duration.tv_usec / 1000));

        char *perf_format = StringConcatenate(2, buffer, format);

        va_list args;
        va_start(args, format);
        vsyslog(LOG_DEBUG, perf_format, args);
        va_end(args);

        free(perf_format);
    }
}
