/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "log.h"

#include "string_lib.h"

#include <stdarg.h>

#ifdef MINGW
void openlog(const char *__ident, int __option, int __facility)
{
    return;
}

void closelog()
{
    return;
}

void syslog(int __pri, const char *__fmt)
{
    return;
}

#else
# include <syslog.h>
#endif

LogPerformanceTimer LogPerformanceStart(void)
{
#ifndef MINGW
    struct timeval start = { 0 };
    struct timezone tz = { 0 };

    if (gettimeofday(&start, &tz) != 0)
    {
        syslog(LOG_DEBUG, "Unable to start timer");
        return start;
    }
    return start;
#else
    return NULL;
#endif
}

void LogPerformanceStop(LogPerformanceTimer *timer, const char *format, ...)
{
#ifndef MINGW
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
#endif
}

const char *LogLevelToString(int level)
{
    switch (level)
    {
    case LOG_EMERG:
        return "emergency";
    case LOG_ALERT:
        return "alert";
    case LOG_CRIT:
        return "critical";
    case LOG_ERR:
        return "error";
    case LOG_WARNING:
        return "warning";
    case LOG_NOTICE:
        return "notice";
    case LOG_INFO:
        return "info";
    case LOG_DEBUG:
        return "debug";
    default:
        return NULL;
    }
}

int LogLevelFromString(const char *level)
{
    if (StringSafeEqual("emergency", level))
    {
        return LOG_EMERG;
    }
    else if (StringSafeEqual("alert", level))
    {
        return LOG_ALERT;
    }
    else if (StringSafeEqual("critical", level))
    {
        return LOG_CRIT;
    }
    else if (StringSafeEqual("error", level))
    {
        return LOG_ERR;
    }
    else if (StringSafeEqual("warning", level))
    {
        return LOG_WARNING;
    }
    else if (StringSafeEqual("notice", level))
    {
        return LOG_NOTICE;
    }
    else if (StringSafeEqual("info", level))
    {
        return LOG_INFO;
    }
    else if (StringSafeEqual("debug", level))
    {
        return LOG_DEBUG;
    }
    else
    {
        return -1;
    }
}
