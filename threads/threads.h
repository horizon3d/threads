#ifndef _INSPIRE_THREAD_BASE_H_
#define _INSPIRE_THREAD_BASE_H_

#ifdef _WINDOWS
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#include "thread.h"
#include "thdMgr.h"
#include "thdEvent.h"
#include "task/thdTask.h"
#include "task/thdMgrTask.h"
#include "task/thdTaskMgr.h"

#endif // _INSPIRE_THREAD_BASE_H_