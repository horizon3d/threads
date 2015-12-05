#ifndef _INSPIRE_UTIL_H_
#define _INSPIRE_UTIL_H_

#include <stdarg.h>
#include <stdio.h>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#endif

#include "util/inspire.h"

inline void utilSleep(int ms)
{
#ifdef _WINDOWS
   ::Sleep(ms);
#else
   usleep(1000 * ms);
#endif
}

inline void yeild(int seconds)
{
#ifdef _WINDOWS
   ::Sleep(seconds * 1000);
#elif _LINUX
#if defined(_PPCLIN64) || defined(_AIX)
   __asm__ __volatile__("or 27, 27, 27");
#else
   __asm__ __volatile__("pause\n": : : "memory");
#endif
#endif
}

inline uint64 utilGetCurrentPid()
{
#ifdef _WINDOWS
   return (uint64)::GetCurrentProcessId();
#else
   return (uint64)getpid();
#endif
}

inline uint64 utilGetCurrentThreadId()
{
#ifdef _WINDOWS
   return (uint64)::GetCurrentThreadId();
#else
   return (uint64)syscall(SYS_gettid);
#endif
}

inline int utilGetLastError()
{
#ifdef _WINDOWS
   return (int)::GetLastError();
#else
   return errno;
#endif
}

inline void Panic()
{
#ifdef _DEBUG
#ifdef _WINDOWS
   __asm int 3;
#else
   __asm__ __volatile__ ("int3");
#endif
#else
   int *p = NULL;
   *p = 1;
#endif
}

inline size_t utilSnprintf(char* buffer, size_t bufferLen, const char* fmt, ...)
{
   va_list ap;
   size_t n;
   va_start(ap, fmt);
#ifdef _WINDOWS
   n = _vsnprintf_s(buffer, bufferLen, _TRUNCATE, fmt, ap);
#else
   n = vsnprintf(buffer, bufferLen, fmt, ap);
#endif
   va_end(ap);

   if (n < 0 || n >= bufferLen)
   {
      n = bufferLen - 1;
   }
   buffer[n] = '\0';
   return n;
}

inline const uint roundUp(const uint original, const uint bytes = 4)
{
   return ((original + (bytes - 1)) - ((original + (bytes - 1)) % bytes));
}

inline const uint roundDown(const uint original, const uint bytes = 4)
{
   return (original / bytes) * bytes;
}

#endif