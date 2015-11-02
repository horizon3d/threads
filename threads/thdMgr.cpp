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

      std::deque<std::pair<int64, threadEntity*> >::iterator it = _idleQueue.begin();
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

   int threadMgr::release(const int64& id)
   {
      int rc = 0;
      std::deque<std::pair<int64, threadEntity*> >::iterator it = _workQueue.begin();
      while ( _workQueue.end() != it)
      {
         if (id == it->first)
         {
            threadEntity* entity = it->second;
            entity->deactive();
            if (entity->poolable())
            {
               _idleQueue.insert(std::make_pair<id, entity>);
            }
            else
            {
               entity->destroy();
            }
         }
      }
   }

}