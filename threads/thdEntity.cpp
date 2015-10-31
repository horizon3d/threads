#include "thdEntity.h"
#include <process.h>
namespace inspire {

   threadEntity::threadEntity(thdType t) : _type(t)
   {
   }

   threadEntity::~threadEntity()
   {
      if (_hThread && _state != STOPPED)
      {
         deactive();
         _state = STOPPED;
         destroy();
      }
   }

   int threadEntity::initialize()
   {
      unsigned threadId = 0;
      _hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntity::ENTRY_POINT, this, CREATE_SUSPENDED, &threadId);
      _tid = threadId;
   }

   int threadEntity::active()
   {
      ::ResumeThread(_hThread);
   }

   int threadEntity::deactive()
   {
      if (STOPPED != _state)
      {
         _state = STOPPED;
         if (WAIT_TIMEOUT == ::WaitForSingleObject(_hThread, 10000))
         {
            destroy();
         }
      }
   }

   int threadEntity::destroy()
   {
      ::_endthreadex(-11);
   }

   int threadEntity::_run()
   {
      while ( STOPPED != _state )
      {
         //TODO:
      }
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