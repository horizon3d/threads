#ifndef _INSPIRE_UTIL_ASSERT_H_
#define _INSPIRE_UTIL_ASSERT_H_

#include "logger/logger.h"
#include "util.hpp"

#ifdef _DEBUG
#define INSPIRE_ASSERT(cond, fmt, ...) \
do                                     \
{                                      \
   if (!(cond))                        \
   {                                   \
      LogError(fmt, ##__VA_ARGS__);    \
      Panic();                         \
   }                                   \
} while (false);
#else
#define INSPIRE_ASSERT(cond, fmt, ...) \
do                                     \
{                                      \
   if (!(cond))                        \
   {}                                  \
} while (false);
#endif

#define STRONG_ASSERT(cond, fmt, ...)  \
do                                     \
{                                      \
   if (!(cond))                        \
   {                                   \
      LogError(fmt, ##__VA_ARGS__);    \
      Panic();                         \
   }                                   \
} while (false);
#endif
