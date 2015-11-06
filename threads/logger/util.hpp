#ifndef _INSPIRE_LOGGER_UTIL_H_
#define _INSPIRE_LOGGER_UTIL_H_

#include <windows.h>

DWORD CurrentPid()
{
#ifdef _WIN32
   return ::GetCurrentProcessId();
#else
   retun getpid();
#endif
}

DWORD CurrentThreadId()
{
#ifdef _WIN32
   return ::GetCurrentThreadId();
#else
   retun syscall(SYS_gettid);
#endif
}

int FetchLastError()
{
#ifdef _WIN32
   return (int)::GetLastError();
#else
   retun errno;
#endif
}


#endif