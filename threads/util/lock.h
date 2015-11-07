#ifndef _INSPIRE_UTIL_LOCK_INTERFACE_H_
#define _INSPIRE_UTIL_LOCK_INTERFACE_H_

namespace inspire {

   class ILock
   {
   public:
      virtual ~ILock() {}
      virtual void lock() = 0;
      virtual void unlock() = 0;
      virtual bool tryLock() = 0;
   };

}
#endif
