/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_LOG_H
#define CFENGINE_LOG_H

#include <platform.h>

#ifdef MINGW
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

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
