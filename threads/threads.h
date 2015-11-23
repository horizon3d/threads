#ifndef _INSPIRE_THREAD_BASE_H_
#define _INSPIRE_THREAD_BASE_H_

#ifdef _WINDOWS
#include <winnt.h>
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

typedef long long int64;
typedef unsigned int uint;
typedef unsigned long long uint64;

#ifdef __cplusplus
#define THREADS_EXPORT_API extern "C" __declspec(dllexport)
#else
#define THREADS_EXPORT_API __declspec(dllexport)
#endif

#include "util/assert.h"

#endif
