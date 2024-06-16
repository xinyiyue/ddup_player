#ifndef __DDUP_LOG_H__
#define __DDUP_LOG_H__

#include <stdarg.h>
#include <stdio.h>

// 定义日志等级
typedef enum LogLevel_ { ERROR, WARNING, INFO, DEBUG } LogLevel;

static LogLevel level_ = INFO;

static const char *get_level_string(LogLevel level) {
  const char *ret;
  switch (level) {
    case ERROR:
      ret = "ERROR";
      break;
    case INFO:
      ret = "INFO";
      break;
    case WARNING:
      ret = "WARNING";
      break;
    case DEBUG:
      ret = "DEBUG";
      break;
    default:
      break;
  }
  return ret;
}

static void log(LogLevel level, const char *tag, const char *format, ...) {
  if (level > level_) return;
  const char *levelStr = get_level_string(level);
  va_list args;
  va_start(args, format);
  fprintf(stdout, "[%s]%s: ", levelStr, tag);
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
  va_end(args);
}

#define LOGE(TAG, FORMART, ...) log(ERROR, TAG, FORMART, __VA_ARGS__)
#define LOGI(TAG, FORMART, ...) log(INFO, TAG, FORMART, __VA_ARGS__)
#define LOGW(TAG, FORMART, ...) log(WARNING, TAG, FORMART, __VA_ARGS__)
#define LOGD(TAG, FORMART, ...) log(DEBUG, TAG, FORMART, __VA_ARGS__)

#endif
