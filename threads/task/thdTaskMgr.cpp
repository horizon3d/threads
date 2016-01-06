/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 tynia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author: tynia
Date  : =========
*******************************************************************************/
#include "thdTaskMgr.h"
#include "thdTask.h"
#include "taskExt.h"

namespace inspire {

   thdTaskMgr* thdTaskMgr::instance()
   {
      static thdTaskMgr mgr;
      return &mgr;
   }

   void thdTaskMgr::registerTask(thdTask* task)
   {
      thdTask* t = NULL;
      if (!_taskMap.fetch(task->id(), t))
      {
         int64 tId = task->id();
         const char* tName = task->name();
         LogEvent("insert a new task into map, id: %lld, name: %s",
                  tId, tName);
         _taskMap.insert(tId, task);
      }
#ifdef _DEBUG
      else
      {
         LogError("try to register a duplicated task!!!"
                  " task id: %d, name: %s, existed task: %s",
                  task->id(), task->name(), t->name());
      }
#endif
   }

   void thdTaskMgr::over(thdTask* task)
   {
      thdTask* t = NULL;
      if (_taskMap.fetch(task->id(), t))
      {
         // TODO: we do not need to fetch a task from the map
         // we have stored the task
         // and the task is the same one we stored
         // let's delete it
         delete task;
         task = NULL;
      }
      else
      {
         // we never stored the task
         // let's mark it unhandled and store it for next use
         task->status(TASK_UNHANDLED);
         _taskMap.insert(task->id(), task);
      }
   }

   thdTask* thdTaskMgr::get(const int64& id)
   {
      thdTask* t = NULL;
      if (_taskMap.fetch(id, t) && TASK_UNHANDLED == t->status())
      {
         // the task is idle, we use it
         return t;
      }
      else
      {
         t = inspire::createTask(id);
      }
      return t;
   }

   void thdTaskMgr::clean()
   {
      std::map<int64, thdTask*>& rmap = _taskMap.raw();
      std::map<int64, thdTask*>::iterator it = rmap.begin();
      for (; rmap.end() != it; ++it)
      {
         thdTask*& t = it->second;
         delete t;
         t = NULL;
      }
   }
}