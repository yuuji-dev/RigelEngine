#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>



 void log_output(LogLevel log_level, const char* message, ...){
  
  const char*level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
  
  char logMessage[32000];
  memset(logMessage, 0, sizeof(logMessage));

  va_list arg_ptr;
  va_start(arg_ptr, message);
  vsnprintf(logMessage, 32000, message, arg_ptr);
  va_end(arg_ptr);
  
  char finalLogMessage[32000];
  
  sprintf(finalLogMessage, "%s%s\n", level_strings[log_level], logMessage);

  printf("%s \n", finalLogMessage);
}
