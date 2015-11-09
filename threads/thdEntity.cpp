#include "thdEntity.h"
#include <process.h>
#include "thdTask.h"
#include "thdMgr.h"
namespace inspire {

   threadEntity::threadEntity(threadMgr* mgr) : _thdMgr(mgr)
   {
   }

   threadEntity::~threadEntity()
   {
      if (_hThread)
      {
         stop();
      }
   }

   int threadEntity::initialize()
   {
      int rc = 0;
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         rc = -1; // system error
         return rc;
      }
      state(THREAD_IDLE);
      _tid = threadId;
      return rc;
   }

   void threadEntity::active()
   {
      state(THREAD_RUNNING);
      ::ResumeThread(_hThread);
   }

   void threadEntity::suspend()
   {
      state(THREAD_IDLE);
      ::SuspendThread(_hThread);
   }

   void threadEntity::resume()
   {
      state(THREAD_RUNNING);
      ::ResumeThread(_hThread);
   }

   int threadEntity::join()
   {
      int rc = 0;
      _task->attach(this);
      rc = _task->run();
      error(rc);
      _task->detach();
      return rc;
   }

   int threadEntity::stop()
   {
      if (THREAD_IDLE == _state)
      {
         state(THREAD_STOPPED);
         // if thread is suspended, we should state it stopped and active it
         active();
      }
      else if (THREAD_STOPPED != _state)
      {
         state(THREAD_STOPPED);
      }

      _task = NULL;
      _thdMgr = NULL;


      return 0;
   }

   int threadEntity::kill()
   {
      ::_endthreadex(-1);
      return 0;
   }

   void threadEntity::wait(int seconds)
   {
      // TODO:
#ifdef _WINDOWS
      YieldProcessor();
#elif _LINUX
   #if defined(_AIX)
      __asm__ __volatile__("pause\n": : : "memory");
   #elif defined(_PPCLIN64)
      __asm__ __volatile__("or 27, 27, 27");
   #elif defined(_AIX)
      __asm__ __volatile__("or 27, 27, 27");
   #endif
#endif
   }

   unsigned __stdcall threadEntity::ENTRY_POINT(void* arg)
   {
      threadEntity* entity = static_cast<threadEntity*>(arg);
      if (entity)
      {
         threadMgr* mgr = entity->thdMgr();
         // assert mgr not NULL
         while (THREAD_RUNNING == entity->state())
         {
            int rc = 0;
            thdTask* task = entity->fetch();
            task->attach(entity);
            rc = task->run();
            if (rc)
            {
               entity->error(rc);
            }
            task->detach();
            mgr->deactive(entity);
         }
         entity->close();
      }

      return 0;
   }

}