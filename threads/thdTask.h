#ifndef _INSPIRE_THREAD_TASK_H_
#define _INSPIRE_THREAD_TASK_H_

#include "threads.h"
#include "entry/entry.h"

namespace inspire {

   class threadEntity;

   class thdTask
   {
      friend class threadEntity;
   public:
      thdTask();
      explicit thdTask(threadEntity* thd);
      virtual ~thdTask();

      virtual int run() = 0;

   public:
      void attach(threadEntity* thd) { _thd = thd; }
      void detach() { _thd->state(THREAD_STOPPING); _thd = NULL; }
      void notify() {}
      void OnBegin() { /* LogEvent begin*/ }
      void OnEnd()   { /* LogEvent end*/ }

   private:
      int64         _taskId;
      threadEntity* _thd;
   };
}
#endif
