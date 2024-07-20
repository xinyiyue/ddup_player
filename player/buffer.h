#ifndef _BUFFER_H__
#define _BUFFER_H__

#define SUPPORTED_PIXEL_FORMAT_COUNT 5

typedef enum PXIEL_FORMART {
  PIXELFORMAT_ARGB8888 = 0,
  PIXELFORMAT_BGRA8888 = 1,
  PIXELFORMAT_NV12 = 2,
  PIXELFORMAT_NV21 = 3,
  PIXELFORMAT_UNKNOWN = 10,
} pixel_format_t;

typedef struct RenderBuffer {
  char *data;
  int len;
  int width;
  int height;
  pixel_format_t pixel;
  int frame_rate;
  long long pts;  // ms
} render_buffer_s;

typedef struct VideoFormat {
  int width;
  int height;
  pixel_format_t pixel[SUPPORTED_PIXEL_FORMAT_COUNT];
} video_format_s;

typedef struct AudioFormat {
  int sample_rate;
  int sample_format;
  int channel_number;
} audio_format_s;

#endif
