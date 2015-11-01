#include "thdMgr.h"

namespace inspire {


   threadMgr* threadMgr::instance()
   {
      static threadMgr mgr;
      return &mgr;
   }

   threadEntity* threadMgr::fetchIdle()
   {
      if (!_idleQueue.size())
      {
         return NULL;
      }

      std::map<int64, threadEntity*>::iterator it = _idleQueue.begin();
      // INSPIRE_ASSERT(_idleQueue.end() != it)
      return it->second;
   }

   int threadMgr::create(thdType t, int64& id)
   {
      int rc = 0;
      threadEntity* entity = new threadEntity(t);
      if (entity)
      {
         // insert into idle ?
         rc = entity->initialize();
         id = _id;
         ++_id;
      }
      return ;
   }

}