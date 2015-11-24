#ifndef _INSPIRE_DEFINITION_H_
#define _INSPIRE_DEFINITION_H_

// include STL
#include <iostream>
#include <vector>
#include <set>
#include <deque>
#include <map>
#include <iterator>
#include <cstring>

typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint;

#ifdef __cplusplus
#define INSPIRE_EXPORT_API extern "C" __declspec(dllexport)
#else
#define INSPIRE_EXPORT_API __declspec(dllexport)
#endif

#endif // _INSPIRE_DEFINITION_H_