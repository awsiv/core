/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "log.h"
#include "string_lib.h"

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
