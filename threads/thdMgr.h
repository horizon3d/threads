#ifndef _INSPIRE_THREAD_MANAGER_H_
#define _INSPIRE_THREAD_MANAGER_H_

#include "util/container/deque.h"
#include "util/container/set.h"
#include "thread.h"
#include "task/thdTask.h"

namespace inspire {

   class thdTask;
   class thdTaskMgr;
   class thdMgr
   {
   private:
      // the event definition
      // the event can be accepted by the thread mgr
      enum EVENT
      {
         EVENT_DUMMY = -1,
         EVENT_DISPATCH_TASK = 0,
         EVENT_THREAD_SUSPEND = 1,
         EVENT_THREAD_RUNNING = 2,
         EVENT_THREAD_RESUME = 3,
         EVENT_THREAD_RELEASE = 4,
         EVENT_THREAD_UPBOUND,
      };

      class thdEvent
      {
      public:
         char  evType;
         void* evObject;

         thdEvent() : evType(EVENT_DUMMY), evObject(NULL) {}
         thdEvent(const char t, void* obj) : evType(t), evObject(obj) {}
         virtual ~thdEvent() {}
         thdEvent(const thdEvent& rhs) : evType(rhs.evType), evObject(rhs.evObject) {}
         thdEvent& operator= (const thdEvent& rhs)
         {
            evType = rhs.evType;
            evObject = rhs.evObject;
            return *this;
         }
      };

      // the main task of thread mgr
      // all event is handle by the task
      // the task id is solid and assigned with 0
      // the id of user-defined tasks should start with 1
      class thdInnerTask : public thdTask
      {
      public:
         thdInnerTask(thdMgr* mgr)
            : thdTask(0, "Event Process Task"), _thdMgr(mgr) {}
         ~thdInnerTask() { _thdMgr = NULL; }

         virtual const int run();

      private:
         thdInnerTask(const thdInnerTask& rhs);
         thdInnerTask& operator= (const thdInnerTask& rhs);

      private:
         thdMgr* _thdMgr;
      };

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
      * set max count of idle thread to be stored(pooled)
      */
      void reverseIdleCount(const uint maxCount = 10);
      /*
      * create a thread
      */
      thread* create();
      /*
      * detach thread from thread map, so that the manager won't manager it
      * user should join, free the thread any more
      */
      void detach(thread* thd);
      /*
      * recycle a thread, it determines a thread is to be suspended or release
      */
      void recycle(thread* thd);
      /*
      * notify thread manager to handle a event
      * return false if program is going exiting
      */
      bool postEvent(const char st, void* pObj);
      /*
      * post a task event to the thread mgr
      */
      bool postEvent(thdTask* task);

   protected:
      /*
      * peek a thread from idle queue
      * create a new thread if idle queue is empty
      */
      thread* _fetchIdle();
      /*
      * push a thread into idle queue for reusing
      */
      void _enIdle(thread* thd);
      /*
      * release a thread
      */
      void _release(thread* thd);
      /*
      * before create a thread, we should get a thread entity pooled in thread queue
      * if a thread exit, the thread entity will be restored for next request
      * this strategy aims at decreasing use of new and delete
      */
      thread* _acquire();
      /*
      * dispatch a task to a thread which is ready
      */
      void _dispatch(thdTask* task);
      

   private:

   private:
      thdMgr();
      thdMgr(const thdMgr& rhs);
      thdMgr& operator=(const thdMgr& rhs);
      virtual ~thdMgr();

   private:
      uint            _maxIdleCount;
      thread*         _thdMain;    ///< special thread for handling event
      thdTaskMgr*     _taskMgr;
      deque<thread*>  _idleQueue;  ///< the queue contains thread entity
      deque<thread*>  _thdQueue;   ///< the queue don't contains thread entity
      set<thread*>    _totalSet;   ///< all thread objects, used for resource management
      deque<thdEvent> _eventQueue;
   };
}
#endif
