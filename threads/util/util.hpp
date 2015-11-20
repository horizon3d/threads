#ifndef _INSPIRE_LOGGER_UTIL_H_
#define _INSPIRE_LOGGER_UTIL_H_

#ifdef _WINDOWS
#include <windows.h>
#else
#endif

inline void inSleep(int ms)
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
   YieldProcessor();
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

inline unsigned long long utilGetCurrentPid()
{
#ifdef _WIN32
   return (unsigned long long)::GetCurrentProcessId();
#else
   retun (unsigned long long)getpid();
#endif
}

inline unsigned long long utilGetCurrentThreadId()
{
#ifdef _WIN32
   return (unsigned long long)::GetCurrentThreadId();
#else
   retun (unsigned long long)syscall(SYS_gettid);
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