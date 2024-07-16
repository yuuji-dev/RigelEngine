#pragma once

#include "defines.h"

#define LOG_DEBUG
#define LOG_INFO

typedef enum LogLevel {
  LOG_LEVEL_FATAL = 0,
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARN  = 2,
  LOG_LEVEL_INFO  = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_TRACE = 5
} LogLevel;

void log_output(LogLevel log_level, const char* message, ...);

#define RFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

#ifdef LOG_DEBUG
#define RDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
#define RDEBUG(message, ...)
#endif

#ifdef LOG_INFO
#define RINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
#define RINFO(message, ...)
#endif




