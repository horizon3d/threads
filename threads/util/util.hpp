#ifndef _INSPIRE_LOGGER_UTIL_H_
#define _INSPIRE_LOGGER_UTIL_H_

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/syacall.h>
#endif

#include "inspire.h"

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
#if defined(_AIX)
   __asm__ __volatile__("pause": : : "memory");
#elif defined(_PPCLIN64)
   __asm__ __volatile__("or 27, 27, 27");
#elif defined(_AIX)
   __asm__ __volatile__("or 27, 27, 27");
#endif
#endif
}

inline uint64 utilGetCurrentPid()
{
#ifdef _WIN32
   return (uint64)::GetCurrentProcessId();
#else
   retun (uint64)getpid();
#endif
}

inline uint64 utilGetCurrentThreadId()
{
#ifdef _WIN32
   return (uint64)::GetCurrentThreadId();
#else
   retun (uint64)syscall(SYS_gettid);
#endif
}

inline int utilGetLastError()
{
#ifdef _WIN32
   return (int)::GetLastError();
#else
   retun errno;
#endif
}

inline void Panic()
{
#ifdef _DEBUG
   __asm int 3;
#else
   int *p = NULL;
   *p = 1;
#endif
}
#endif