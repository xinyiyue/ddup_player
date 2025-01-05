
#include  "base_util/media_type.h"
#include <string.h>

int is_media_file(const char *filename) {
  const char *a_extensions[] = {".mp3", ".wav",  ".aac", ".flac",
                                ".pcm", ".opus", NULL};

  const char *v_extensions[] = {".mp4", ".avi", ".mov", ".mkv", NULL};

  const char *p_extensions[] = {".png", ".jpg", ".webp", ".gif", NULL};

  const char *dot = strrchr(filename, '.');
  if (dot == NULL) {
    return false;
  }
  size_t len = strlen(dot);
  for (int i = 0; a_extensions[i] != NULL; i++) {
    if (len == strlen(a_extensions[i]) &&
        strncasecmp(dot, a_extensions[i], len) == 0) {
      return 2;
    }
  }

  for (int i = 0; v_extensions[i] != NULL; i++) {
    if (len == strlen(v_extensions[i]) &&
        strncasecmp(dot, v_extensions[i], len) == 0) {
      return 1;
    }
  }

  for (int i = 0; p_extensions[i] != NULL; i++) {
    if (len == strlen(p_extensions[i]) &&
        strncasecmp(dot, p_extensions[i], len) == 0) {
      return 3;
    }
  }
  return -1;
}
