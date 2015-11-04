#ifndef _INSPIRE_UTIL_CONDITION_VARIABLE_H_
#define _INSPIRE_UTIL_CONDITION_VARIABLE_H_

#include "spinlock.h"

namespace inspire {

   class condition_t
   {
   public:
      condition_t(spinlock_t* spin) : _spin(spin) { _spin->lock(); }
      ~condition_t() { _spin->unlock(); }

   private:
      condition_t(const condition_t& rhs) = delete;
      condition_t& operator= (const condition_t& rhs) = delete;

   private:
      spinlock_t* _spin;
   };
}
#endif