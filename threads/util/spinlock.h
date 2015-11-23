#ifndef _INSPIRE_UTIL_SPIN_LOCK_H_
#define _INSPIRE_UTIL_SPIN_LOCK_H_

#include "inspire.h"
#include "lock.h"

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
         } while (InterlockedCompareExchange(&_spin, LOCK, UNLOCK) && times > 0);
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
         } while (InterlockedCompareExchange(&_spin, LOCK, UNLOCK));
      }

      virtual void unlock()
      {
         InterlockedExchange(&_spin, UNLOCK);
      }

   private:
      unsigned volatile _spin;
   };
}
#endif
