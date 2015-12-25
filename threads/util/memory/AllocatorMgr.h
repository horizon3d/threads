#ifndef _INSPIRE_UTIL_ALLOCATOR_MGR_H_
#define _INSPIRE_UTIL_ALLOCATOR_MGR_H_

#include "util/inspire.h"
#include "util/system/mutex.h"

namespace inspire {

   class AllocatorMgr
   {
   private:
      AllocatorMgr();
      virtual ~AllocatorMgr();

   public:
      static AllocatorMgr* instance();
      void* alloc(const uint size, const char* file, const uint line);
      void* realloc(void* ptr, const uint size, const char* file, const uint line);
      void  dealloc(void* ptr);
      void  pray();

   private:
      void* _alloc(const uint size, const char* file, const uint line);
      uint  _locate(const uint size);
      void  _setSanity(void* ptr, const uint size);
      bool  _checkSanity(void* ptr);
#ifdef _DEBUG
      void  _debug(void* ptr, const char* file, const uint line);
#endif

      static const uint64 magic = 0xFFFAFBFE;
      struct header
      {
         char    eyecatcher[8]; // inspire + '\0'                       8 bytes
         header* next;          // next buffer header                 4/8 bytes
         uint    size;          // valid size can be used               4 bytes
         uint    magic;         // magic number                         4 bytes
#ifdef _DEBUG
         uint line;
         const char* file;
#endif
      };

   private:
      struct freelist
      {
         uint    size;
         mutex_t mtx;
         header* hdr;
      };

      enum {
         MEMORY_SIZE_INCREMENT   = 8,
         MAX_ALLOCATOR_COUNT     = 17,
         MAX_LIMITED_MEMORY_SIZE = 128,
      };
      //index: 0   1   2   3   4   5   6   7   8   9   10  11  12   13   14   15  16
      //size : 8   16  24  32  40  48  56  64  72  80  88  86 104  112  120  128  x
      freelist _fls[MAX_ALLOCATOR_COUNT];               // free lists
   };
}
#endif