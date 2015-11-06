#ifndef _INSPIRE_LOGGER_WRITER_H_
#define _INSPIRE_LOGGER_WRITER_H_

#include "base.h"

namespace inspire {

   class spinlock_t;

   namespace logger {

      class writerImpl : public logWriter
      {
      public:
#ifdef _DEBUG
         writerImpl(const int priority = LOG_DEBUG);
#else
         writerImpl(const int priority = LOG_ERROR);
#endif
         virtual ~writerImpl() {};

         virtual void writeLog(const int priority, const char* data);

      protected:
         void initialize();

      protected:
         int  _priority;
         spinlock_t *_spin;
         char _filename[MAX_LOG_FILE_NAME + 1];
      };
   }
}
#endif