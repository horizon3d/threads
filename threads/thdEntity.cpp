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
      if (_hThread && state() != THREAD_STOPPED)
      {
         deactive();
      }
   }

   int threadEntity::initialize()
   {
      int rc = 0;
      _state = THREAD_CREATING;
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         rc = -1; // system error
         state(THREAD_INVALID);
         return rc;
      }
      _state = THREAD_IDLE;
      _tid = threadId;
      return rc;
   }

   int threadEntity::active()
   {
      state(THREAD_RUNNING);
      return ::ResumeThread(_hThread);
   }

   int threadEntity::suspend()
   {
      state(THREAD_SUSPEND);
      return ::SuspendThread(_hThread);
   }

   int threadEntity::resume()
   {
      state(THREAD_RUNNING);
      return ::ResumeThread(_hThread);
   }

   int threadEntity::join()
   {
      int rc = ::WaitForSingleObject(_hThread, INFINITE);
      return rc;
   }

   int threadEntity::deactive()
   {
      state(THREAD_STOPPING);
      _task = NULL;
   }

   int threadEntity::destroy()
   {
      int rc = 0;
      state(THREAD_STOPPING);
      if (WAIT_TIMEOUT == ::WaitForSingleObject(_hThread, 10000))
      {
         _state = THREAD_STOPPED;
         ::_endthreadex(-11);
         state(THREAD_STOPPED);
      }
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

   int threadEntity::_run()
   {
      int rc = 0;
      while ( THREAD_STOPPED != _state )
      {
         //TODO:
         _task->run();
      }
      _state = THREAD_STOPPED;
      return rc;
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
            thdTask* task = mgr->fetchTask();
            if (NULL == task)
            {
               Sleep(2000);
               continue;
            }

            int rc = 0;
            task->attach(entity);
            rc = task->run();
            if (rc)
            {
               //LogEvent(...);
               entity->err(rc);
            }
            task->detach();
         }
         entity->_run();
      }
   }

}