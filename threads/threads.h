#ifndef _INSPIRE_THREAD_BASE_H_
#define _INSPIRE_THREAD_BASE_H_



#ifdef _WIN32
#include <windows.h>
#else
#include <sys/thread>
#endif

typedef long long int64;
typedef unsigned int uint;
typedef unsigned long long uint64;
typedef int64 TID;
typedef DWORD PID;

#ifdef __cplusplus
#define THREADS_EXPORT_API extern "C" __declspec(dllexport)
#else
#define THREADS_EXPORT_API __declspec(dllexport)
#endif

#endif
