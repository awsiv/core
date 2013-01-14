/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "log.h"
#include "string_lib.h"

#include <stdarg.h>

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

LogPerformanceTimer LogPerformanceStart(void)
{
    return NULL;
}

void LogPerformanceStop(LogPerformanceTimer *timer, const char *format, ...)
{
}
