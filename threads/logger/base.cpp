#include <ctime>
#include <stdarg.h>
#include <stdio.h>

#include "base.h"
#include "util/util.hpp"

namespace inspire {

   namespace logger {

#ifdef _WIN32
      const static char *logFmt = \
         "%04d-%02d-%02d-%02d.%02d.%02dLevel: %-37s\r\nPID: %-37dTID:  %d" \
         "\r\nFunction: %-32sLine: %d\r\nFile: %s\r\nMessage:\r\n%s\r\n\r\n";
#else
      const static char *logFmt = \
         "%04d-%02d-%02d-%02d.%02d.%02d\nLevel: %-37s\nPID: %-37dTID:  %d" \
         "\nFunction: %-32sLine: %d\nFile: %s\nMessage:\r\n%s\n\n";
#endif

      const char* toString(const unsigned priority)
      {
         static char* typeMap[] = {
            "Always", "Emergency", "Fatal", "Critical", "Error",
            "Event", "Debug", "Warning", "Notice", "Info"
         };

         if (priority < 0 || priority > 900)
            return "Unknown priority";
         else
            return typeMap[priority / 100];
      }

      void append(const unsigned level, const char* func,
                  const char* file, const unsigned line,
                  const char* fmt, ...)
      {
         struct tm otm;
         time_t tt = time(NULL);
         ::localtime_s(&otm, &tt);

         char userInfo[LOG_BUFFER_SIZE] = { 0 };
         va_list ap;
         va_start(ap, fmt);
         vsprintf_s(userInfo, LOG_BUFFER_SIZE, fmt, ap);
         va_end(ap);

         char buffer[LOG_BUFFER_SIZE] = { 0 };
         sprintf_s(buffer, LOG_BUFFER_SIZE, logFmt,
                   otm.tm_year + 1900, otm.tm_mon + 1, otm.tm_mday,
                   otm.tm_hour, otm.tm_min, otm.tm_sec,
                   toString(level), CurrentPid(), CurrentThreadId(),
                   func, line, file, userInfo);
         loggerWriter()->writeLog(level, buffer);
      }
   }
}