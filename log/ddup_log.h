#ifndef __DDUP_LOG_H__
#define __DDUP_LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include "player/util.h"

// 定义日志等级
typedef enum LogLevel_ { ERROR, WARNING, INFO, DEBUG } LogLevel;

static LogLevel level_ = INFO;

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static const char *get_level_string(LogLevel level) {
  const char *ret;
  switch (level) {
    case ERROR:
      ret = "E";
      break;
    case INFO:
      ret = "I";
      break;
    case WARNING:
      ret = "W";
      break;
    case DEBUG:
      ret = "D";
      break;
    default:
      break;
  }
  return ret;
}

static void log(LogLevel level, const char *tag, const char *format, ...) {
  if (level > level_) return;
  AutoLock lock(&log_mutex);
  const char *levelStr = get_level_string(level);
  va_list args;
  va_start(args, format);
  fprintf(stdout, "[%s][%s]:", levelStr, tag);
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
  va_end(args);
}

#define LOGE(TAG, FORMAT, ...) log(ERROR, TAG, FORMAT, __VA_ARGS__)
#define LOGI(TAG, FORMAT, ...) log(INFO, TAG, FORMAT, __VA_ARGS__)
#define LOGW(TAG, FORMAT, ...) log(WARNING, TAG, FORMAT, __VA_ARGS__)
#define LOGD(TAG, FORMAT, ...) log(DEBUG, TAG, FORMAT, __VA_ARGS__)

#endif
