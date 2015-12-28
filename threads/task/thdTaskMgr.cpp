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
      if (_taskMap.fetch(task->id(), t) && t == task)
      {
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