#ifndef _INSPIRE_LOGGER_UTIL_H_
#define _INSPIRE_LOGGER_UTIL_H_

#ifdef _WINDOWS
#include <windows.h>
#else
#endif

inline unsigned CurrentPid()
{
#ifdef _WIN32
   return (unsigned )::GetCurrentProcessId();
#else
   retun (unsigned)getpid();
#endif
}

inline unsigned CurrentThreadId()
{
#ifdef _WIN32
   return (unsigned)::GetCurrentThreadId();
#else
   retun (unsigned)syscall(SYS_gettid);
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