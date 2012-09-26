/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_LOG_H
#define CFENGINE_LOG_H

#include <platform.h>

#ifdef MINGW
void openlog(const char *__ident, int __option, int __facility);
void closelog();
void syslog(int __pri, const char *__fmt);

typedef void *LogPerformanceTimer;
#else
typedef struct timeval LogPerformanceTimer;
#endif

const char *LogLevelToString(int level);
int LogLevelFromString(const char *level);

LogPerformanceTimer LogPerformanceStart(void);
void LogPerformanceStop(LogPerformanceTimer *timer, const char *format, ...);


#endif
