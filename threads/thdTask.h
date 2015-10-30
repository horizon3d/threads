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
      explicit thdTask();
      virtual ~thdTask();

      virtual int run() = 0;
   protected:
      void setOwner(threadEntity* thd);

   private:
      int64 _taskId;
      threadEntity* _owner;
      THREAD_ENTRY  _entry;
   };
}
#endif
