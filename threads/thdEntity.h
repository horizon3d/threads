#ifndef _INSPIRE_THREAD_ENTITY_H_
#define _INSPIRE_THREAD_ENTITY_H_

#include "threads.h"

namespace inspire {

   enum thdState
   {
      THREAD_INVALID  = 0,
      THREAD_CREATING = 1,
      THREAD_IDLE     = 2,
      THREAD_RUNNING  = 3,
      THREAD_STOPPING = 4,
      THREAD_STOPPED  = 5,
   };

   enum thdType
   {
      THREAD_SYSTEM,
      THREAD_SERVICE,
      THREAD_WORK,     // can be pooled
   };

   //class thdTask;
   class threadEntity
   {
      static unsigned __stdcall ENTRY_POINT(void* arg);
   public:
      threadEntity(thdType t);
      virtual ~threadEntity();

      int initialize();
      int active();
      int deactive();
      int destroy();

      const int64 tid() const { return _tid; }

      bool poolable() const { _pooled; }

   private:
      int _run();

   public:
      static int64 _entityId;

   private:
      bool     _pooled;
      int      _state = THREAD_INVALID;
      int64    _tid = -1;
      HANDLE   _hThread = INVALID_HANDLE_VALUE;
      //thdTask* _task;

   private:
      threadEntity(const threadEntity& rhs) = delete;
      threadEntity& operator= (const threadEntity& rhs) = delete;
   };
}
#endif
