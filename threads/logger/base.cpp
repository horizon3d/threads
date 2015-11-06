#include <ctime>
#include <stdarg.h>
#include <stdio.h>

#include "base.h"
#include "util.hpp"

namespace inspire {

   namespace logger {

      const static char *logFmt = "%04d-%02d-%02d-%02d.%02d.%02d"NEWLINE
                                  "PID: %-37dTID: %d"NEWLINE
                                  "Level: %s"NEWLINE"Function: %-32s"NEWLINE
                                  "File: %s"NEWLINE"Line: %d"NEWLINE
                                  "Message:"NEWLINE"%s"NEWLINE NEWLINE;

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
                   func, file, line, userInfo);
         loggerWriter()->writeLog(level, buffer);
      }
   }
}