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
      
#ifdef _WINDOWS
      mutex_t(const char* name = NULL)
      {
         CharConvertor uname(name);
         _hMutex = CreateEvent(NULL, FALSE, TRUE, uname.toString());
         if (INVALID_HANDLE_VALUE == _hMutex)
#else
      mutex_t()
      {
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
         _hMutex = INVALID_HANDLE_VALUE;
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
#ifdef _WINDOWS
      const char* name = NULL;
      HANDLE _hMutex = INVALID_HANDLE_VALUE;
#else
      pthread_mutex_t _hMutex = PTHREAD_MUTEX_INITIALIZER;
#endif
   };
}
#endif
