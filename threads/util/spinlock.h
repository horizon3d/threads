#ifndef _INSPIRE_UTIL_SPIN_LOCK_H_
#define _INSPIRE_UTIL_SPIN_LOCK_H_

#include "lock.h"
#include "atomic.h"

namespace inspire {

   class spinlock_t : public ILock
   {
      static const unsigned LOCK = 1;
      static const unsigned UNLOCK = 0;
   public:
      spinlock_t() : _spin(0) {}
      virtual ~spinlock_t() {}

      virtual bool tryLock()
      {
         int times = 11;
         do 
         {
            --times;
         } while (utilCompareAndSwap32(&_spin, LOCK, UNLOCK) && times > 0);
         if (times > 0)
            return true;
         return false;
      }

      virtual void lock()
      {
         unsigned pin = 0;
         do 
         {
            ++pin;
         } while (utilCompareAndSwap32(&_spin, LOCK, UNLOCK));
      }

      virtual void unlock()
      {
         utilAtomicExchange32(&_spin, UNLOCK);
      }

   private:
      int volatile _spin;
   };
}
#endif
