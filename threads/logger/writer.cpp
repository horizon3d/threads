#include <ctime>
#include "writer.h"
#include "util/spinlock.h"
#include "util/condition.h"
#include <stdio.h>

namespace inspire {

   namespace logger {

      writerImpl::writerImpl(const int priority) : _priority(priority)
      {
         _spin = new inspire::spinlock_t();
      }

      writerImpl::~writerImpl()
      {
         delete _spin;
         _spin = NULL;
      }

      void writerImpl::writeLog(const int priority, const char* data)
      {
         if (_priority <= priority)
         {
            return;
         }

         inspire::condition_t cond(_spin);
         std::fstream file;
         file.open(_filename, std::ios::out | std::ios::app);
         if (file.is_open())
         {
            file.tellg();
            file.write(data, strlen(data));
            file.close();
         }
      }

      void writerImpl::initialize()
      {
         struct tm otm;
         time_t tt = time(NULL);
         ::localtime_s(&otm, &tt);

         sprintf_s(_filename, LOG_BUFFER_SIZE, "%04d-%02d-%02d-%02d.%02d.%02d.log",
                   otm.tm_year + 1900, otm.tm_mon + 1, otm.tm_yday,
                   otm.tm_hour, otm.tm_min, otm.tm_sec);
      }

      static writerImpl writer;
      logWriter* loggerWriter()
      {
         return &writer;
      }
   }
}