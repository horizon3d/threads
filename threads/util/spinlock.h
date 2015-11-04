#ifndef _INSPIRE_UTIL_SPIN_LOCK_H_
#define _INSPIRE_UTIL_SPIN_LOCK_H_

namespace inspire {

   class spinlock_t
   {
      static const uint LOCK = 1;
      static const uint UNLOCK = 0;
   public:
      spinlock_t() { _spin = new uint(0); }
      virtual ~spinlock_t() { delete _spin; _spin = NULL; }

      bool tryLock()
      {
         return (UNLOCK == InterlockedCompareExchange(_spin, LOCK, UNLOCK));
      }

      void lock()
      {
         uint pin = 0;
         do 
         {
            ++pin;
         } while (InterlockedCompareExchange(_spin, LOCK, UNLOCK));
      }

      void unlock()
      {
         InterlockedExchange(_spin, UNLOCK)
      }

   private:
      uint * volatile _spin;
   };
}
#endif
