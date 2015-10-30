#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "threads.h"

namespace inspire {

   enum thdState
   {
      INVALID  = 0,
      CREATING = 1,
      SUSPEND  = 2,
      RUNNING  = 3,
      STOPPING = 4,
      STOPPED  = 5,
   };

   typedef unsigned(__stdcall *ENTRY_FUNC)(void*);

   class threadEntity
   {
      static unsigned __stdcall ENTRY_POINT(void* arg);
   public:
      threadEntity();
      virtual ~threadEntity();

      int active();
      int deactive();

      const int64 id() const { return _tid; }

   private:
      int _create();
      int _destroy();
      int _run();

   private:
      int      _state = INVALID;
      int64    _tid = -1;
      HANDLE   _hThread = INVALID_HANDLE_VALUE;
      thdTask* _task;

   private:
      threadEntity(const threadEntity& rhs) = delete;
      threadEntity& operator= (const threadEntity& rhs) = delete;
   };
}
#endif
