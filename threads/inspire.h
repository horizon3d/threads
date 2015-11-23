#ifndef _INSPIRE_DEFINITION_H_
#define _INSPIRE_DEFINITION_H_

// include STL
#include <vector>
#include <set>
#include <deque>
#include <map>

#ifdef _WINDOWS
#include <winnt.h>
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

typedef long long int64;
typedef unsigned int uint;
typedef unsigned long long uint64;

#ifdef __cplusplus
#define INSPIRE_EXPORT_API extern "C" __declspec(dllexport)
#else
#define INSPIRE_EXPORT_API __declspec(dllexport)
#endif

#include "util/assert.h"

#endif // _INSPIRE_DEFINITION_H_