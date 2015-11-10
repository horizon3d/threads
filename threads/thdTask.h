#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "threads.h"
#include "thdEntity.h"
#include "thdTaskMgr.h"

namespace inspire {

   enum
   {
      TASK_UNHANDLED = 0,
      TASK_HANDLEING = 1,
      TASK_HANDLED = 2,
   };

   class thdTask
   {
   public:
      thdTask(const int64& id, const char* name) : _status(TASK_UNHANDLED), _taskId(id), _thd(NULL), _name(name)
      {
         thdTaskMgr::instance()->registerTask(this);
      }
      explicit thdTask(const int64& id, const char* name, threadEntity* thd) : _status(TASK_UNHANDLED), _taskId(id), _thd(thd), _name(name) {}
      virtual ~thdTask() {}

      virtual int run() = 0;

      virtual const char* name() { return _name; };

   public:
      const int64 id() const { return _taskId; }
      const uint status() const { return _status; }
      void status(const uint st) { _status = st; }
      void attach(threadEntity* thd = NULL)
      {
         if(NULL != thd)
         {
            _thd = thd;
         }
         OnBegin();
      }
      void detach() { OnEnd(); }

   private:
      void OnBegin()
      {
         status(TASK_HANDLEING);
         LogEvent("Task: %lld begin, current thread id: %d", _taskId, _thd->tid());
      }

      void OnEnd()
      {
         status(TASK_HANDLED);
         _thd = NULL;
         LogEvent("Task: %lld over", _taskId);
      }

   private:
      uint          _status;
      int64         _taskId;
      const char*   _name;
      threadEntity* _thd;
   };
}
#endif
