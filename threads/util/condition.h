#ifndef _INSPIRE_UTIL_CONDITION_VARIABLE_H_
#define _INSPIRE_UTIL_CONDITION_VARIABLE_H_

#include "lock.h"

namespace inspire {

   class condition_t
   {
   public:
      condition_t(ILock* locker) : _locker(locker) { _locker->lock(); }
      ~condition_t() { _locker->unlock(); }

   private:
      condition_t(const condition_t& rhs) = delete;
      condition_t& operator= (const condition_t& rhs) = delete;

   private:
      ILock* _locker;
   };
}
#endif