#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include "util/container/deque.h"
#include "util/container/set.h"
#include "thread.h"
#include "thdEvent.h"

namespace inspire {

   class thdTask;
   class thdTaskMgr;
   class thdMgr
   {
   public:
      void initialize();
      void active();
      void destroy();

      static thdMgr* instance();
      /*
      * receive event and handle event
      */
      void process();
      /*
      * set max count of idle thread to be stored
      */
      void reverseIdleCount(const uint maxCount = 10);

      /*
      * create a thread
      */
      thread* create();
      /*
      * notify thread manager to handle a event
      * return false if program is going exiting
      * more event detail, defined in thdEvent.h
      */
      bool notify(const char st, void* pObj);
      /*
      * recycle a thread, it determines a thread is to be suspended or release
      */
      void recycle(thread* thd);

   protected:
      /*
      * peek a thread from idle queue
      * create a new thread if idle queue is empty
      */
      thread* fetchIdle();
      /*
      * push a thread into idle queue for reusing
      */
      void enIdle(thread* thd);
      /*
      * release a thread
      */
      void release(thread* thd);
      /*
      * before create a thread, we should get a thread entity pooled in thread queue
      * if a thread exit, the thread entity will be restored for next request
      * this strategy aims at decreasing use of new and delete
      */
      thread* acquire();
      /*
      * dispatch a task to a thread which is ready
      */
      void dispatch(thdTask* task);
      /*
      * detach thread from thread map, so that the manager won't manager it
      * user should join, free the thread any more
      */
      void detach(thread* thd);

   private:
      thdMgr();
      thdMgr(const thdMgr& rhs);
      thdMgr& operator=(const thdMgr& rhs);
      virtual ~thdMgr();

   private:
      uint            _maxIdleCount;
      thread*         _mThd;       ///< special thread for handling event
      thdTaskMgr*     _taskMgr;
      deque<thread*>  _idleQueue;  ///< the queue contains thread entity
      deque<thread*>  _thdQueue;   ///< the queue don't contains thread entity
      set<thread*>    _totalSet;   ///< all thread objects, used for resource management
      deque<thdEvent> _eventQueue;
   };
}
#endif
