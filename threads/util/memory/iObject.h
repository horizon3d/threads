#ifndef _INSPIRE_BASE_OBJECT_H_
#define _INSPIRE_BASE_OBJECT_H_

#include "util/inspire.h"
#include "util/memory/AllocatorMgr.h"

namespace inspire {

   #define INSPIRE_NEW new(__FILE__, __LINE__, std::nothrow)
   #define INSPIRE_DEL delete
   #define INSPIRE_SAFE_DEL(p) \
   do                          \
   {                           \
      if (NULL != p)           \
      {                        \
         INSPIRE_DEL p;        \
         p = NULL;             \
      }                        \
   } while (0);
   #define INSPIRE_MALLOC(x) INSPIRE_INNER_MALLOC(x, __FILE__, __LINE__)
   #define INSPIRE_FREE(x)   AllocatorMgr::instance()->dealloc(p)
   #define INSPIRE_SAFE_FREE(p)  \
   do                            \
   {                             \
      if (NULL != p)             \
      {                          \
         INSPIRE_DEL p;          \
         p = NULL;               \
      }                          \
   } while (0);

   class iObject
   {
      #define INSPIRE_INNER_MALLOC(x, y, z) AllocatorMgr::instance()->alloc(x, y, z)
      #define INSPIRE_MALLOC(x) INSPIRE_INNER_MALLOC(x, __FILE__, __LINE__)
      #define INSPIRE_FREE(p)   AllocatorMgr::instance()->dealloc(p)
   protected:
      iObject() {}
      virtual ~iObject() {}

   public:
      void* operator new(uint size) throw (const char*)
      {
         void* ptr = INSPIRE_MALLOC(size);
         if (NULL == ptr)
            throw "Allocate Failed";
         return ptr;
      }

      void* operator new[](uint size) throw (const char*)
      {
         void* ptr = INSPIRE_MALLOC(size);
         if (NULL == ptr)
            throw "Allocate Failed";
         return ptr;
      }

      void operator delete(void *p)
      {
         INSPIRE_FREE(p);
      }

      void operator delete[](void *p)
      {
         INSPIRE_FREE(p);
      }

      void * operator new (uint size, const char *file, uint line) throw (const char *)
      {
         void *p = INSPIRE_INNER_MALLOC(size, file, line);
         if (!p)
            throw "Allocate Failed";
         return p;
      }

      void * operator new[](uint size, const char *file, uint line) throw (const char *)
      {
         void *p = INSPIRE_INNER_MALLOC(size, file, line);
         if (!p)
            throw "Allocate Failed";
         return p;
      }

      void operator delete(void *p, const char *file, uint line)
      {
         INSPIRE_FREE(p);
      }

      void operator delete[](void *p, const char *file, uint line)
      {
         INSPIRE_FREE(p);
      }

      void * operator new(uint size, const std::nothrow_t &)
      {
         return INSPIRE_MALLOC(size);
      }

      void * operator new[](uint size, const std::nothrow_t &)
      {
         return INSPIRE_MALLOC(size);
      }

      void operator delete(void *p, const std::nothrow_t &)
      {
         INSPIRE_FREE(p);
      }

      void operator delete[](void *p, const std::nothrow_t &)
      {
         INSPIRE_FREE(p);
      }

      void * operator new(uint size, const char *file,
                           uint line, const std::nothrow_t &)
      {
         return INSPIRE_INNER_MALLOC(size, file, line);
      }

      void * operator new[](uint size, const char *file,
                            uint line, const std::nothrow_t &)
      {
         return INSPIRE_INNER_MALLOC(size, file, line);
      }

      void operator delete(void *p, const char *file,
                           uint line, const std::nothrow_t &)
      {
         INSPIRE_FREE(p);
      }

      void operator delete[](void *p, const char *file,
                             uint line, const std::nothrow_t &)
      {
         INSPIRE_FREE(p);
      }
   };
}
#endif