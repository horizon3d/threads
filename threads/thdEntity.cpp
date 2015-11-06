#include "thdEntity.h"
#include <process.h>
#include "thdTask.h"
#include "thdMgr.h"
namespace inspire {

   threadEntity::threadEntity(threadMgr* mgr, bool worker) : _worker(worker), _thdMgr(mgr)
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
      state(THREAD_CREATING);
      unsigned threadId = 0;
      if (_worker)
      {
         _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::WORKER_ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      }
      else
      {
         _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      }
      if (INVALID_HANDLE_VALUE == _hThread)
      {
         rc = -1; // system error
         state(THREAD_INVALID);
         return rc;
      }
      state(THREAD_IDLE);
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
      state(THREAD_IDLE);
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

   void threadEntity::deactive()
   {
      _task = NULL;
      state(THREAD_STOPPING);
   }

   void threadEntity::destroy()
   {
      state(THREAD_STOPPED);
      if (WAIT_TIMEOUT == ::WaitForSingleObject(_hThread, 10000))
      {
         err(-11); // time out, kill force
         ::_endthreadex(-11);
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

   unsigned __stdcall threadEntity::WORKER_ENTRY_POINT(void* arg)
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
               entity->wait(2000);
               continue;
            }
            // LogEvent fetch a task
            int rc = 0;
            task->attach(entity);
            rc = task->run();
            entity->err(rc);
            task->detach();
         }
      }

      return 0;
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
            entity->err(rc);
            task->detach();
            mgr->recycle(entity);
         }
      }

      return 0;
   }

}