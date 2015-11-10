#ifndef _INSPIRE_LOGGER_UTIL_H_
#define _INSPIRE_LOGGER_UTIL_H_

#ifdef _WINDOWS
#include <windows.h>
#else
#endif

inline unsigned long long CurrentPid()
{
#ifdef _WIN32
   return (unsigned long long)::GetCurrentProcessId();
#else
   retun (unsigned long long)getpid();
#endif
}

inline unsigned long long CurrentThreadId()
{
#ifdef _WIN32
   return (unsigned long long)::GetCurrentThreadId();
#else
   retun (unsigned long long)syscall(SYS_gettid);
#endif
}

inline int FetchLastError()
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