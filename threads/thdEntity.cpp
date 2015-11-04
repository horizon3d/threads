#include "thdEntity.h"
#include <process.h>
#include "thdTask.h"
namespace inspire {

   threadEntity::threadEntity()
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
      ::ResumeThread(_hThread);
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
         entity->_run();
      }
   }

}