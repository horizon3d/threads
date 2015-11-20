#ifndef _INSPIRE_THREAD_EVENT_H_
#define _INSPIRE_THREAD_EVENT_H_

#include "threads.h"

namespace inspire {

   enum EVENT
   {
      EVENT_DUMMY          = -1,
      EVENT_DISPATCH_TASK  = 0,
      EVENT_THREAD_SUSPEND = 1,
      EVENT_THREAD_RUNNING = 2,
      EVENT_THREAD_RESUME  = 3,
      EVENT_THREAD_RELEASE = 4,
      EVENT_THREAD_UPBOUND,
   };

   class thread;
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
}
#endif
