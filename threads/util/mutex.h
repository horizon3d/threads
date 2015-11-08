#ifndef _INSPIRE_UTIL_MUTEX_H_
#define _INSPIRE_UTIL_MUTEX_H_

#include <windows.h>
#include "lock.h"
#include "util.hpp"
#include "charConvertor.h"

namespace inspire {

   class mutex_t : public ILock
   {
   public:
      mutex_t(const char* name = NULL)
      {
#ifdef _WINDOWS
         CharConvertor uname(name);
         _hMutex = CreateEvent(NULL, FALSE, TRUE, uname.toString());
         if (INVALID_HANDLE_VALUE == _hMutex)
#else
         int res = pthread_mutex_init(&m, NULL);
         if (res)
#endif
         {
            Panic();
         }
      }

      ~mutex_t()
      {
#ifdef _WINDOWS
         ::CloseHandle(_hMutex);
#else
         int res;
         do 
         {
            res = pthread_mutex_destroy(&m);
         } while (EINTR == res);
#endif
      }

   public:
      virtual void lock()
      {
#ifdef _WINDOWS
         DWORD dw = ::WaitForSingleObject(_hMutex, INFINITE);
         if (WAIT_OBJECT_0 == dw)
         {
            //::ResetEvent(_hMutex);
         }
#else
         int res;
         do
         {
            res = pthread_mutex_lock(&_hMutex);
         } while (EINTR == res);
         if (res)
         {
            Panic();
         }
#endif
      }
      virtual void unlock()
      {
#ifdef _WINDOWS
         ::SetEvent(_hMutex);
#else
         int res;
         do
         {
            res = pthread_mutex_unlock(&_hMutex);
         } while (EINTR == res);
         if (res)
         {
            Panic();
         }
#endif
      }
      virtual bool tryLock()
      {
         bool ok = true;
#ifdef _WINDOWS
         DWORD dw = ::WaitForSingleObject(_hMutex, 200);
         if (WAIT_TIMEOUT == dw)
         {
            ok = false;
         }
         else if (WAIT_OBJECT_0 == dw)
         {
            //::ResetEvent(_hMutex);
         }
         else
         {
            Panic();
         }
#else
         int res;
         do
         {
            res = pthread_mutex_trylock(&_hMutex);
         } while (EINTR == res);
         if (res && (EBUSY != res))
         {
            ok = false;
         }
#endif
         return ok;
      }

   private:
      const char* name;
#ifdef _WINDOWS
      HANDLE _hMutex;
#else
      pthread_mutex_t _hMutex;
#endif
   };
}
#endif
