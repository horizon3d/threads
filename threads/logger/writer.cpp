#include <ctime>
#include <stdio.h>
#include "writer.h"
#include "util/spinlock.h"
#include "util/condition.h"
#include "util/ossFile.h"

namespace inspire {

   namespace logger {

      writerImpl::writerImpl(const int priority) : _priority(priority)
      {
         _spin = new inspire::spinlock_t();
         initialize();
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

         unsigned len = strlen(data);
         unsigned written = 0;
         condition_t cond(_spin);
         _logger->open(_filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         _logger->seekToEnd();
         _logger->write(data, len + 1, len, written);
         _logger->close();
      }

      void writerImpl::initialize()
      {
         int rc = 0;
         struct tm otm;
         time_t tt = time(NULL);
         ::localtime_s(&otm, &tt);
         char filename[MAX_LOG_FILE_NAME + 1] = { 0 };
         sprintf_s(_filename, LOG_BUFFER_SIZE, "%04d-%02d-%02d-%02d.%02d.%02d.log",
                   otm.tm_year + 1900, otm.tm_mon + 1, otm.tm_yday,
                   otm.tm_hour, otm.tm_min, otm.tm_sec);
         _logger = new ossFile();
         rc = _logger->open(_filename, MODE_CREATE | ACCESS_READWRITE, DEFAULT_FILE_ACCESS);
         if (rc)
         {
            // TODO:
         }
         _logger->close();
      }

      static writerImpl writer;
      logWriter* loggerWriter()
      {
         return &writer;
      }
   }
}