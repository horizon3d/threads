#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "thread.h"
#include "thdTaskMgr.h"

namespace inspire {

   enum
   {
      TASK_UNHANDLED = 0,
      TASK_RUNNING   = 1,
      TASK_HANDLED   = 2,
   };

   typedef void (*TASK_END_FUNC)(void* result);
   class thdTask
   {
   public:
      thdTask(const int64& id, const char* name)
         : _status(TASK_UNHANDLED), _taskId(id), _name(name), _thd(NULL), _cb(NULL)
      {
         thdTaskMgr::instance()->registerTask(this);
      }
      explicit thdTask(const int64& id, const char* name, thread* thd)
         : _status(TASK_UNHANDLED), _taskId(id), _name(name), _thd(thd), _cb(NULL)
      {}
      virtual ~thdTask() {}

      virtual const int run() = 0;

   public:
      const char* name() { return _name; };
      const int64 id() const { return _taskId; }
      const uint status() const { return _status; }
      void status(const uint st) { _status = st; }
      void attach(thread* thd = NULL)
      {
         if(NULL != thd)
         {
            _thd = thd;
         }
         status(TASK_RUNNING);
         LogEvent("Task: %lld begin handling", _taskId, _thd->tid());
      }

      void detach()
      {
         if (NULL != _cb)
         {
            // need to be finish parameter
            // _cb(NULL);
         }
         status(TASK_HANDLED);
         LogEvent("Task: %lld over", _taskId);
         _thd = NULL;
      }

      TASK_END_FUNC OnTaskEnd(TASK_END_FUNC cb)
      {
         TASK_END_FUNC old = _cb;
         _cb = cb;
         return old;
      }

   protected:
      uint          _status;
      int64         _taskId;
      const char*   _name;
      thread*       _thd;
      TASK_END_FUNC _cb;
   };
}
#endif
