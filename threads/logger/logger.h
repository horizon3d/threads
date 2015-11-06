#ifndef _INSPIRE_LOGGER_H_
#define _INSPIRE_LOGGER_H_

#include "base.h"
// \this file is used for other to include to use Log...

#define __LOG_WRAPPER(LEVEL, fmt, ...)                               \
do                                                                   \
{                                                                    \
   inspire::logger::append(LEVEL, __FUNCTION__, __FILE__,            \
                           __LINE__, fmt, __VA_ARGS__);              \
} while (false);

#define LogEmerg(fmt, ...) \
__LOG_WRAPPER(LOG_EMERG, fmt, __VA_AGRS__)

#define LogFatal(fmt, ...) \
__LOG_WRAPPER(LOG_FATAL, fmt, __VA_ARGS__)

#define LogCritial(fmt, ...) \
__LOG_WRAPPER(LOG_CRITICAL, fmt, __VA_ARGS__)

#define LogError(fmt, ...) \
__LOG_WRAPPER(LOG_ERROR, fmt, __VA_ARGS__)

#define LogEvent(fmt, ...) \
__LOG_WRAPPER(LOG_EVENT, fmt, __VA_ARGS__)

#define LogWarning(fmt, ...) \
__LOG_WRAPPER(LOG_WARNING, fmt, __VA_ARGS__)

#define LogDebug(fmt, ...) \
__LOG_WRAPPER(LOG_DEBUG, fmt, __VA_ARGS__)

#define LogNotice(fmt, ...) \
__LOG_WRAPPER(LOG_NOTICE, fmt, __VA_ARGS__)

#define LogInfo(fmt, ...) \
__LOG_WRAPPER(LOG_INFO, fmt, __VA_ARGS__)

#endif